
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
const char* serverName = "https://secrets-api.appbrewery.com/random" ; 

const char* ssid = "Frankys Home";
const char* password = "osoNDU120.";

String sensorReadings;
float sensorReadingsArr[5];
String payload;

  
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
  
void setup() {
  
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
}



  
void loop() {
  
 
 //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      HTTPClient http;
  
    http.begin(serverName); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
  
    if (httpCode > 0) { //Check for the returning code
  
        payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }
  
    else {
      Serial.println("Error on HTTP request");
    }
  
    http.end(); //Free the resources
    
const size_t bufferSize = JSON_OBJECT_SIZE(10);
  DynamicJsonDocument doc(1024);
  //json = msg.c_str();
  DeserializationError error = deserializeJson(doc, payload);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

int id = doc["id"];
const char* secret = doc["secret"];
int emScore = doc["emScore"];
const char* username = doc["username"];
const char* timestampp = doc["timestamp"];


  Serial.println (id);
 Serial.println (secret) ;
 Serial.println (emScore) ;
 Serial.println (username) ;
Serial.println   (timestampp);
    
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  
}
















/*
void connectToWifi()
{
  WiFi.begin(ssid, password);
  String dots[3] = {".", "..", "..."};
  int numberOfDots = 1;

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to Wifi");



    if (numberOfDots == 3)
    {
      numberOfDots = 0;
    }
    else
    {
      numberOfDots++;
    }

    delay(300);
  }

  Serial.println("Connected!!......");

}

*/
