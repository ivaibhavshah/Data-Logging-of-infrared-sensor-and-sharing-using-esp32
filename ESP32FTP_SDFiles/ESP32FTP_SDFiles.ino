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

uint64_t uS_TO_S_FACTOR = 1000000; 
uint64_t TIME_TO_SLEEP = 2;

const char* ssid     = "Redmi Note 8 Pro";
const char* password = "11111111";

RTC_DATA_ATTR int sensor_data = 0;
String Data;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String Date;
String day;
String Time;

// Function to read data from Ultra sonic sensor
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
  if (distance < 12){
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" Inch");  

  }

}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

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
  ReadData();
  Serial.print("Data readed");
  // File file = SD.open("/temperature_readings.txt");
  // if(!file) {
  //   Serial.println("File does not exist");
  //   Serial.println("Creating file...");
  //   writeFile(SD, "/temperature_readings.txt", "Reading Number, Date, Hour, Temperature \r\n");
  // }
  // file.close();
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // timeClient.update();
  printLocalTime();
  Serial.print("Date obtained");

  data_logging();
  sensor_data++;
  Serial.println("Sensor data logged successfully! Going to sleep");
  // delay(000);
  // esp_deep_sleep_start(); 
}


const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
const char* ntpServer = "asia.pool.ntp.org";

void printLocalTime()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Time = String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec) ;
  
  Serial.println(Time);
}

void data_logging() {
  Data = String(sensor_data) + "," + String(day) + "," + String(Time) + "," + 
                String(distance) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(Data);
  appendFile(SD, "/temperature_readings.txt", Data.c_str());
}

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
