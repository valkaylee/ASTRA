#include <WiFi.h>
#include <ESP32WebServer.h>
#include "SD_MMC.h"
#include "CSS.h"
#include "sd_read_write.h"
#include <Wire.h>
#include "MS5837.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Servo.h"

// Constants and setup code 
#define SD_MMC_CMD 15
#define SD_MMC_CLK 14
#define SD_MMC_D0  2
#define TDS_PIN 34
#define MOTOR_PIN 19
#define BUTTON_PIN 13

const char *ssid  =  "kayleeAP";
const char *password =  "12345678";

String count;
unsigned long lastLogTime = 0;
unsigned long motorStartTime = 0;
unsigned long totalMotorTime = 0;
bool motorDirection = true; // true = clockwise, false = counterclockwise
bool motorRunning = false;
bool cycleComplete = false;
MS5837 sensor;
Servo motorServo;
ESP32WebServer server(80);
TaskHandle_t Task1;
TaskHandle_t Task2;
File UploadFile;

/*********  SETUP  **********/
void setup() {
  Serial.begin(115200);
  motorServo.attach(MOTOR_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // WiFi setup
  Serial.println("Setting up WiFi...");
  WiFi.softAP(ssid, password);
  Serial.print("Connect to access point: ");
  Serial.println(ssid);
  Serial.println(String("Soft-AP IP address = ") + WiFi.softAPIP().toString());

  // Start HTTP server
  Serial.println("Starting HTTP server...");
  server.on("/", SD_dir);
  server.on("/upload", File_Upload);
  server.on("/fupload", HTTP_POST, [](){ server.send(200); }, handleFileUpload);
  
  // Handle file download
  server.on("/download", [](){
    String fileName = server.arg("file");
    if (fileName.length() > 0) {
      File download = SD_MMC.open("/" + fileName);
      if (download) {
        server.sendHeader("Content-Type", "application/octet-stream");
        server.sendHeader("Content-Disposition", "attachment; filename=" + fileName);
        server.sendHeader("Connection", "close");
        server.streamFile(download, "application/octet-stream");
        download.close();
      } else {
        server.send(404, "text/plain", "File not found");
      }
    }
  });

  // Handle file delete
  server.on("/delete", [](){
    String fileName = server.arg("file");
    if (fileName.length() > 0) {
      if (SD_MMC.remove("/" + fileName)) {
        server.send(200, "text/html", "<h3>File " + fileName + " deleted successfully.</h3><a href='/'>Go Back</a>");
      } else {
        server.send(500, "text/html", "<h3>Failed to delete file " + fileName + ".</h3><a href='/'>Go Back</a>");
      }
    }
  });

  server.begin();
  Serial.println("HTTP server started");

  // SD card setup
  Serial.print(F("Initializing SD card..."));
  SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
  if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)){
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }
  deleteFile(SD_MMC, "/my_float_data.txt");

  // Sensor setup
  Wire.begin();
  while (!sensor.init()) {
    Serial.println("Sensor init failed!");
    delay(5000);
  }

  // Write the initial headers to the SD card file
  writeFile(SD_MMC, "/my_float_data.txt", "Time\tDepth\tPressure\tTemperature\tTDS\n");

  // Start motor
  motorServo.write(0);
  motorStartTime = millis();
  motorRunning = true;

  // Create tasks for motor control and sensor logging
  xTaskCreatePinnedToCore(Task1code, "MotorControlTask", 10000, NULL, 1, &Task1, 0);
  delay(500);
  xTaskCreatePinnedToCore(Task2code, "SensorLoggingTask", 10000, NULL, 2, &Task2, 1);
  delay(500);
}

/*********  LOOP  **********/
void loop() {
  server.handleClient();
  delay(2);
}

/*********  Task1: Motor Control  **********/
void Task1code(void *pvParameters) {
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());

    while (1) {
        if (cycleComplete) {
            vTaskDelete(NULL);  // End the task if the cycle is complete
        }

        unsigned long currentTime = millis();

        if (motorRunning) {
            if (motorDirection) {  // If the motor is moving downward (counterclockwise)
                motorServo.write(0);  // Move downward (counterclockwise)
                
                if (digitalRead(BUTTON_PIN) == LOW) {  // Button pressed
                    motorServo.write(90);  // Stop the motor
                    totalMotorTime = currentTime - motorStartTime;  // Calculate the time it took to press the button

                    // Reverse the motor direction
                    motorDirection = false;
                    motorStartTime = millis();  // Reset the start time for the upward movement
                }
            } else {  // If the motor is moving upward (clockwise)
                motorServo.write(180);  // Move upward (clockwise)
                
                if (currentTime - motorStartTime >= totalMotorTime) {  // Time to stop after moving up for the same duration
                    motorServo.write(90);  // Stop the motor
                    motorRunning = false;  // End the motor cycle
                    cycleComplete = true;  // Mark the cycle as complete
                }
            }
        }
        delay(100);
    }
}

/*********  Task2: Sensor Data Logging  **********/
void Task2code(void * pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  while (1) {
    if (cycleComplete) {
      vTaskDelete(NULL);
    }

    unsigned long currentTime = millis();
    if (currentTime - lastLogTime >= 1000) {
      logData();
      lastLogTime = currentTime;
    }
    delay(100);
  }
}

/*********  FUNCTIONS  **********/
void logData() {
  sensor.read();
  String depth = String(sensor.depth(), 2);
  String pressure = String(sensor.pressure(), 2);
  float tempC = sensor.temperature();
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  String temperature = String(tempF, 2);
  String tds = String(analogRead(TDS_PIN));

  count = String(millis() / 1000);
  String data = count + "\t" + depth + "\t" + pressure + "\t" + temperature + "\t" + tds + "\n";
  appendFile(SD_MMC, "/my_float_data.txt", data.c_str());
}

// SD directory listing
void SD_dir() {
    File root = SD_MMC.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        server.send(500, "text/plain", "Failed to open directory");
        return;
    }

    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        server.send(500, "text/plain", "Not a directory");
        return;
    }

    append_page_header();  // Start HTML

    // Start constructing the HTML for the webpage
    webpage += "<table border='1' cellpadding='5'><tr><th>File Name</th><th>Size</th><th>Actions</th></tr>";

    // Iterate over all files and add them to the table
    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        String fileSize = String(file.size());

        // Construct the row with Download and Delete buttons
        webpage += "<tr>";
        webpage += "<td>" + fileName + "</td>";
        webpage += "<td>" + fileSize + " bytes</td>";
        webpage += "<td>";
        webpage += "<a href='/download?file=" + fileName + "' class='rcorners_n'>Download</a>&nbsp;";
        webpage += "<a href='/delete?file=" + fileName + "' class='rcorners_n' onclick='return confirm(\"Are you sure you want to delete this file?\");'>Delete</a>";
        webpage += "</td>";
        webpage += "</tr>";

        // Move to the next file
        file = root.openNextFile();
    }

    // Close the table and finish the HTML
    webpage += "</table>";

    append_page_footer();  // Close HTML

    // Send the generated webpage to the client
    server.send(200, "text/html", webpage);
}



// File upload handling
void File_Upload() {
  append_page_header();
  webpage += F("<h3>Select File to Upload</h3>");
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:25%' type='file' name='fupload' id = 'fupload' value=''>");
  webpage += F("<button class='buttons' style='width:10%' type='submit'>Upload File</button><br><br>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  append_page_footer();
  server.send(200, "text/html", webpage);
}

// Handle actual file upload process
void handleFileUpload() {
  HTTPUpload& uploadfile = server.upload();
  if (uploadfile.status == UPLOAD_FILE_START) {
    String filename = uploadfile.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    SD_MMC.remove(filename);
    UploadFile = SD_MMC.open(filename, FILE_WRITE);
  } else if (uploadfile.status == UPLOAD_FILE_WRITE) {
    if (UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize);
  } else if (uploadfile.status == UPLOAD_FILE_END) {
    if (UploadFile) {
      UploadFile.close();
      append_page_header();
      webpage += F("<h3>File was successfully uploaded</h3>");
      webpage += F("<h2>Uploaded File Name: "); webpage += uploadfile.filename + "</h2>";
      webpage += F("<h2>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br><br>";
      append_page_footer();
      server.send(200, "text/html", webpage);
    } else {
      webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>");
      server.send(200, "text/html", webpage);
    }
  }
}

// Utility to convert file sizes to human-readable format
String file_size(int bytes) {
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes) + " B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3) + " KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3) + " MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3) + " GB";
  return fsize;
}
