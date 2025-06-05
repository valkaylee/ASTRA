String webpage = ""; //String to save the html code
void append_page_header() {
webpage = F("<!DOCTYPE html><html>");
webpage += F("<head>");
webpage += F("<title>ASTRA</title>"); // NOTE: 1em = 16px
webpage += F("<meta name='viewport'
content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
webpage += F("<style>");//From here style:
webpage += F("body{max-width:65%;margin:0 auto;font-family:Arial,
sans-serif;font-size:100%;background-color:#f0f8ff; color:#333;}");
webpage +=
F("ul{list-style-type:none;padding:0;border-radius:0.5em;overflow:hidden;background-co
lor:#1e90ff;font-size:1em;}");
webpage += F("li{float:left;border-radius:0.5em;border-right:0em solid #bbb;}");
webpage += F("li a{color:white; display:block;border-radius:0.375em;padding:0.44em
0.44em;text-decoration:none;font-size:100%;}");
webpage += F("li
a:hover{background-color:#0073e6;border-radius:0.375em;font-size:100%;}");
webpage += F("h1{color:white;border-radius:0.375em;font-size:1.5em;padding:0.2em
0.2em;background:#1e90ff;text-align:center;}");
webpage += F("h2{color:#1e90ff;font-size:1em;margin-top:1em;}");
webpage += F("h3{font-size:0.9em;color:#333;}");
webpage += F("table{font-family:Arial,
sans-serif;font-size:0.9em;border-collapse:collapse;width:85%;margin:1em auto;}");
webpage += F("th,td {border:0.06em solid
#dddddd;text-align:left;padding:0.6em;border-bottom:0.06em solid #dddddd;}");
webpage += F("th {background-color:#1e90ff;color:white;}");
webpage += F("tr:nth-child(odd) {background-color:#f2f2f2;}");
webpage += F(".rcorners_n {border-radius:0.5em;background:#0073e6;padding:0.6em
0.6em;width:20%;color:white;font-size:75%;}");
webpage += F(".rcorners_m {border-radius:0.5em;background:#0073e6;padding:0.6em
0.6em;width:50%;color:white;font-size:75%;}");
webpage += F(".rcorners_w {border-radius:0.5em;background:#0073e6;padding:0.6em
0.6em;width:70%;color:white;font-size:75%;}");
webpage += F(".column{float:left;width:50%;height:45%;}");
webpage += F(".row:after{content:'';display:table;clear:both;}");
webpage += F("*{box-sizing:border-box;}");
webpage += F("a{font-size:85%;color:#1e90ff;text-decoration:none;}");
webpage += F("a:hover{text-decoration:underline;}");
webpage += F("p{font-size:85%;color:#333;}");
webpage += F("</style></head><body><h1>Float Data</h1>");
webpage += F("<ul>");
webpage += F("<li><a href='/'>Files</a></li>"); //Menu bar with commands
webpage += F("<li><a href='/upload'>Configuration</a></li>");
webpage += F("</ul>");
}
//Saves repeating many lines of code for HTML page footers
void append_page_footer()
{
webpage += F("</body></html>");
}
