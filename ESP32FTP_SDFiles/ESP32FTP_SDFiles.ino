
// Libraries to get time from NTP Server
#include <WiFi.h>
#include "time.h"
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>
// Define CS pin for the SD card module
#define SD_CS 5


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
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// defines variables
const int trigPin = 16;
const int echoPin = 17;
long duration;
int distance;

// Replace with your network credentials
const char* ssid     = "Green PG";
const char* password = "jayhind303";



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
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("cm");  

}
void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  SD_WifiInit();
  printLocalTime();
  /*
  %A	returns day of week
  %B	returns month of year
  %d	returns day of month
  %Y	returns year
  %H	returns hour
  %M	returns minutes
  %S	returns seconds
  */
}
void loop() {
  //ReadData();
  delay(20);
}


void SD_WifiInit(){
Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");


  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
   
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  if (SD.begin(SD_CS)){
    Serial.println("Card Mounted Successfully!!");
  } 
}  