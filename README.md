# Meet ASTRA
## Adaptive Submersible for Terrestrial and Relational Analysis 

### Abstract

Our Adaptive Submersible for Terrestrial and Relational Analysis (ASTRA) is a vertical profiling float designed for environmental monitoring in large bodies of water. ASTRA features a robust PETG 3D-printed lid, securely mounted to a commercially available bottle, ensuring both durability and resilience. Inside, a continuous rotation DC motor initially turns clockwise, allowing the syringe mechanism to manipulate ballast weight and adjust internal water volume, causing the float to descend. After 35 seconds, the motor activates a button, signaling the float to stop for 5 seconds before returning back to the surface. 

Throughout the sinking and rising, ASTRA takes in data from its Blue Robotics Bar02 Ultra High Resolution 10m Depth/Pressure Sensor and Water Quality Monitoring Sensor, which measures depth, pressure, temperature, and total dissolved solids. 

The ESP32 Wrover S3 microcontroller, the brain of ASTRA, controls all of these functions. Our code stored in the ESP32 manages the motor and button tasks, while also processing data from the sensors. For effective communication, ASTRA utilizes an ESP32 access point to transmit the collected numbers in a readable format. Measurements are initially saved to an onboard SD card and then accessed through a webpage via an ESP32 access point, eliminating the need for WiFi and enabling real-time monitoring and remote access to environmental metrics.

By providing detailed data of underwater environments that are difficult for humans to reach, ASTRA advances earth science and underwater ROV technology, ultimately furthering conservation efforts and scientific research. 

<p align="center">
  <img src="https://github.com/user-attachments/assets/0e319652-bb97-473a-8588-ad9f065d94bd" width="45%" />
  <img src="https://github.com/user-attachments/assets/bb517866-33f3-47fe-bef7-127bc25c87e2" width="45%" />
</p>

