
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
const char* serverName = "http://ondigo.us-east-1.elasticbeanstalk.com/payment" ;
const char* serverName2 = "https://secrets-api.appbrewery.com/register" ; 

const char* ssid = "Franky's Note20 Ultra";
const char* password = "OSOndu150";

String sensorReadings;
float sensorReadingsArr[5];
String payload;
HTTPClient http;
  
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
  
 /*
 //Send an HTTP POST request every 10 minutes
 // if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){


  
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


    /*
const size_t bufferSize = JSON_OBJECT_SIZE(10);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

int id =                 doc["id"];
const char* secret =     doc["secret"];
int emScore =            doc["emScore"];
const char* username =   doc["username"];
const char* timestampp = doc["timestamp"];


Serial.println (id);
Serial.println (secret) ;
Serial.println (emScore) ;
Serial.println (username) ;
Serial.println (timestampp);

}
else {
      Serial.println("WiFi Disconnected");
    }
   /* lastTime = millis();
  }*/
  




//delay (5000);

/////////////////////////////////////////////////////////////////////////////////
  // Serializing in JSON Format
  DynamicJsonDocument docc(1024);
  docc["UID"] = "frankyjayde";
  docc["amount"] = "500";
  docc["machineId"] = "dres500";
  String msg;
  serializeJson(docc, msg);
  Serial.println(msg);


if(WiFi.status()== WL_CONNECTED){

    http.begin(serverName); //Specify the URL
    http.addHeader("Content-Type", "application/json");

 
// Send the POST request
   int httpCode = http.POST(msg);

    if (httpCode > 0) {
      Serial.print("HTTP response code: ");
      Serial.println(httpCode);

      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpCode);
      Serial.println("Failed to make POST request");
    }

    http.end();

}

else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();

  

delay (5000);
      
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
