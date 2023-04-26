#include <WiFi.h>              //Built-in
#include <WiFiMulti.h>
#include <ESP32WebServer.h>    //https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <ESPmDNS.h>
#include "time.h"
#include "CSS.h" //Includes headers of the web and de style file
#include <SD.h> 
#include <SPI.h>
#include "FS.h"
#define trigPin  4
#define echoPin  17
#define CALCULATION_TO_CONVERT 0.006692917
#define led 2

const char * message;
long duration;
float distance;
char filename[50];
char dir[10];
char filename_wt[50];
// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;
String dataMessage;

ESP32WebServer server(80);
WiFiMulti wifiMulti;
#define servername "esp32server" //Define the name to your server... 
#define SD_pin 16 //G16 

bool   SD_present = false; //Controls if the SD card is present or not


// variables to get time from ntp server of GMT +5.30
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;
const char* ntpServer = "asia.pool.ntp.org";

String Data;
String DateTime;
String Dirname;
String Time;

/*********  SETUP  **********/

void setup()
{  
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  wifiMulti.addAP("Green PG","jayhind303");  // add Wi-Fi networks you want to connect to, it connects strongest to weakest
  wifiMulti.addAP("Redmi Note 8 Pro", "11111111");  // Adjust the values in the Network tab
  wifiMulti.addAP("Techsture 2020", "Tech7219@@");
  wifiMulti.addAP("POCO M3", "knightmoon");
  
  Serial.println("Connecting ...");


  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250); Serial.print('.');
  }
  Serial.println("\nConnected to "+WiFi.SSID()+" Use IP address: "+WiFi.localIP().toString()); 
  // Setting mdns with server name
  if (!MDNS.begin(servername)) 
  {          
    Serial.println(F("Error setting up MDNS responder!")); 
    ESP.restart(); 
  } 

  Serial.print(F("Initializing SD card..."));
  
  //see if the card is present and can be initialised.
  if (!SD.begin(SD_pin))
  { 
    Serial.println(F("Card failed or not present, no SD Card data logging possible..."));
    SD_present = false; 
  } 
  else
  {
    Serial.println(F("Card initialised... file access enabled..."));
    SD_present = true; 
  }
  
  /*********  Server Commands  **********/
  server.on("/",         SD_dir);
  server.on("/upload",   File_Upload);
  server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, handleFileUpload);

  server.begin();
  
  Serial.println("HTTP server started");
}

/*********  LOOP  **********/

void loop()
{
  server.handleClient(); //Listen for client connections
  getTime();

  ReadData();
  if (distance<20)
  {
    digitalWrite(led, 1);
    logSDCard();
    digitalWrite(led, 0);
  }
  delay(200);


}

void getTime(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // There was an error in pre defined function for getting year soo we variable for getting year only is used
  char timeYear[5];
  strftime(timeYear, 5, "%Y", &timeinfo);
  DateTime = String(timeYear) + "-" + String((timeinfo.tm_mon) + 1) + "-" + String(timeinfo.tm_mday)+" "+String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min);
  Dirname = String(timeYear) + String((timeinfo.tm_mon) +1)+ String(timeinfo.tm_mday);
  String temp = "/" + Dirname;
  temp.toCharArray(dir, 10);
  temp="/"+Dirname+".txt";
  temp.toCharArray(filename_wt, 50);
   DateTime+=":" + String(timeinfo.tm_sec);

}

// Write the sensor readings on the SD card
void logSDCard() {

  
  readingID++;
  dataMessage = String(readingID) + "," + String(DateTime) + "," + 
                String(distance) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);

  File file = SD.open(filename_wt);
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, filename_wt, "Reading ID, DateTime, Distance \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  
  appendFile(SD, filename_wt, dataMessage.c_str());
  }


// Write to the SD card (DON'T MODIFY THIS FUNCTION)

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void ReadData(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distance = duration * CALCULATION_TO_CONVERT;
  
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (inch): ");
  Serial.println(distance);
}



/*********  FUNCTIONS  **********/
//Initial page of the server web, list directory and give you the chance of deleting and uploading



void SD_dir()
{
  if (SD_present) 
  {
    //Action acording to post, dowload or delete, by MC 2022
    if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
    { 
      Serial.println(server.arg(0));
  
      String Order = server.arg(0);
      Serial.println(Order);
      
      if (Order.indexOf("download_")>=0)
      {
        Order.remove(0,9);
        SD_file_download(Order);
        Serial.println(Order);
      }
  
      if ((server.arg(0)).indexOf("delete_")>=0)
      {
        Order.remove(0,7);
        SD_file_delete(Order);
        Serial.println(Order);
      }
    }

    File root = SD.open("/");
    if (root) {
      root.rewindDirectory();
      SendHTML_Header();    
      webpage += F("<div class='dashboard'>");
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th><th>Download</th><th>Delete</th></tr>");
      printDirectory("/",0);
      webpage += F("</table>");
      webpage += F("</div>");
      SendHTML_Content();
      root.close();
    }
    else 
    {
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
    }
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   //Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}

//Upload a file to the SD
void File_Upload()
{
  append_page_header();
  webpage += F("<h3 class='h-text'>Select File to Upload</h3>"); 
  webpage += F("<FORM class='form-upload' action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input type='file' name='fupload' id = 'fupload' value=''>");
  webpage += F("<button type='submit' class='upload-btn'>Upload File</button><br><br>");
  webpage += F("<a href='/'><button class='back'>Back</button></a><br><br>");
  append_page_footer();
  server.send(200, "text/html",webpage);
}

//Prints the directory, it is called in void SD_dir() 
void printDirectory(const char * dirname, uint8_t levels)
{
  
  File root = SD.open(dirname);

  if(!root){
    return;
  }
  if(!root.isDirectory()){
    return;
  }
  File file = root.openNextFile();

  int i = 0;
  while(file){
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if(file.isDirectory()){
      // webpage += "<tr><td>"+String(file.isDirectory()?"Dir":"File")+"</td><td>"+String(file.name())+"</td><td></td></tr>";
      printDirectory(file.name(), levels-1);
    }
    else
    {
      webpage += "<tr><td>"+String(file.name())+"</td>";
      webpage += "<td>"+String(file.isDirectory()?"Dir":"File")+"</td>";
      int bytes = file.size();
      String fsize = "";
      if (bytes < 1024)                     fsize = String(bytes)+" B";
      else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      webpage += "<td>"+fsize+"</td>";
      webpage += "<td>";
      webpage += F("<FORM action='/' method='post'>"); 
      webpage += F("<button type='submit' class='download' name='download'"); 
      webpage += F("' value='"); webpage +="download_"+String(file.name()); webpage +=F("'>Download</button>");
      webpage += "</td>";
      webpage += "<td>";
      webpage += F("<FORM action='/' method='post'>"); 
      webpage += F("<button type='submit' class='delete' name='delete'"); 
      webpage += F("' value='"); webpage +="delete_"+String(file.name()); webpage +=F("'>Delete</button>");
      webpage += "</td>";
      webpage += "</tr>";

    }
    file = root.openNextFile();
    i++;
  }
  file.close();

 
}

//Download a file from the SD, it is called in void SD_dir()
void SD_file_download(String filename)
{
  if (SD_present) 
  { 
    File download = SD.open("/"+filename);
    if (download) 
    {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download"); 
  } else ReportSDNotPresent();
}

//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload()
{ 
  HTTPUpload& uploadfile = server.upload(); //See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            //For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          //If the file was successfully created
    {                                    
      UploadFile.close();   //Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      webpage = "";
      append_page_header();
      webpage += F("<h3 class='h-text'>File was successfully uploaded</h3>"); 
      webpage += F("<h2 class='h-text'>Uploaded File Name: "); webpage += uploadfile.filename+"</h2>";
      webpage += F("<h2 class='h-text'>File Size: "); webpage += file_size(uploadfile.totalSize) + "</h2><br><br>"; 
      webpage += F("<a href='/'><button class='back'>Back</button</a><br>");
      append_page_footer();
      server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}

//Delete a file from the SD, it is called in void SD_dir()
void SD_file_delete(String filename) 
{ 
  if (SD_present) { 
    SendHTML_Header();
    File dataFile = SD.open("/"+filename, FILE_READ); //Now read data from SD Card 
    if (dataFile)
    {
      if (SD.remove("/"+filename)) {
        Serial.println(F("File deleted successfully"));
        webpage += "<h3 class='h-text'>File '"+filename+"' has been deleted</h3>"; 
        webpage += F("<a href='/'><button class='back'>Back</button></a><br><br>");
      }
      else
      { 
        webpage += F("<h3 class='h-text'>File was not deleted - error</h3>");
        webpage += F("<a href='/'><button class='back'>Back</button></a><br>");
      }
    } else ReportFileNotPresent("delete");
    append_page_footer(); 
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSDNotPresent();
} 

//SendHTML_Header
void SendHTML_Header()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  server.sendHeader("Pragma", "no-cache"); 
  server.sendHeader("Expires", "-1"); 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  server.send(200, "text/html", ""); //Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Content
void SendHTML_Content()
{
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Stop
void SendHTML_Stop()
{
  server.sendContent("");
  server.client().stop(); //Stop is needed because no content length was sent
}

//ReportSDNotPresent
void ReportSDNotPresent()
{
  SendHTML_Header();
  webpage += F("<h3 class='h-text'>No SD Card present</h3>"); 
  webpage += F("<a href='/'><button class='back'>Back</button></a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportFileNotPresent
void ReportFileNotPresent(String target)
{
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'><button class='back'>Back<button></a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportCouldNotCreateFile
void ReportCouldNotCreateFile(String target)
{
  SendHTML_Header();
  webpage += F("<h3 class='h-text'>Could Not Create Uploaded File (write-protected?)</h3>"); 
  webpage += F("<a href='/"); webpage += target + "'><button class='back'>Back</button></a><br><br>";
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//File size conversion
String file_size(int bytes)
{
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}
