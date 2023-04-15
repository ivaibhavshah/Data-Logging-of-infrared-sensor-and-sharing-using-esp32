#include "FS.h"
#include "SD.h"

#include "time.h"
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// defines variables
const int trigPin = 16;
const int echoPin = 17;
long duration;
int distance;
char filename[50];
uint64_t uS_TO_S_FACTOR = 1000000; 
uint64_t TIME_TO_SLEEP = 2;

// variables to get time from ntp server of GMT +5.30
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
const char* ntpServer = "asia.pool.ntp.org";

const char* ssid     = "Redmi Note 8 Pro";
const char* password = "11111111";

RTC_DATA_ATTR int sensor_data = 0;
String Data;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String Date;
String Day;
String Time;
const int led = 2;

// Function to read data from Ultra sonic sensor

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  printLocalTime();
  timeClient.begin();
  timeClient.setTimeOffset(18000);

  if(!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  
}

void loop() {
  Serial.println("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    return;    
  }
  else {
    Serial.println("SD Card Initialized!!");
  }
  ReadData();
  
  if (distance<20){
    File file = SD.open(filename);
    if(!file) {
      Serial.println("File does not exist");
      Serial.println("Creating file...");
      writeFile(SD, filename , "Reading Number, Date, Hour, Distance \r\n");
    }
    file.close();
    printLocalTime();
    data_logging();
    sensor_data++;
  }
  delay(1000);
  
}

void ReadData() {
    // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance= duration*0.034/2;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" Inch");  
}


void printLocalTime(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // There was an error in pre defined function for getting year soo we variable for getting year only is used
  char timeYear[5];
  strftime(timeYear,5, "%Y", &timeinfo);
  Time = String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec) ;
  Day =  String(timeYear)+"-"+String((timeinfo.tm_mon)+1)+"-"+ String(timeinfo.tm_mday);
  String temp = "/"+Day+".txt";
  temp.toCharArray(filename, 50);

}

void data_logging() {
  digitalWrite(led, 1);
  Data = String(sensor_data) + "," + String(Day) + "," + String(Time) + "," + 
                String(distance) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(Data);
  appendFile(SD,filename, Data.c_str());
  digitalWrite(led, 0);
}

void writeFile(fs::FS &fs,char filename[50], const char* message) {
  Serial.printf("Writing file: %s\n", filename);

  File file = fs.open(filename, FILE_WRITE);
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

void appendFile(fs::FS &fs,char filename[50], const char * message) {
  Serial.printf("Appending to file: %s\n", filename);

  File file = fs.open(filename, FILE_APPEND);
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
