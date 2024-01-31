
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 1); //GPS (RX, TX)

String message_with_data;

// Variables for storing GPS Data
float latitude;
float longitude;
float speed;
float satellites;
String direction;


//static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

//SoftwareSerial ss(RXPin, TXPin);


unsigned int move_index = 1;

void setup()
{
  // Set console baud rate
  mySerial.begin(9600);
  Serial.begin(115200);
  Serial.println("GPS start");
  delay(10);

}
void checkGPS()
{
  if (gps.charsProcessed() < 10)
  {
   Serial.println(F("No GPS detected: check wiring."));
   
  }
}

void loop()
{
  while (mySerial.available() > 0)
  {
    if (gps.encode(mySerial.read()))
      displayInfo();
  }
}

void displayInfo()
{

  if (gps.location.isValid() )
  {

    latitude = (gps.location.lat());     //Storing the Lat. and Lon.
    longitude = (gps.location.lng());

    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
   Serial.print("LONG: ");
   Serial.println(longitude, 6);
    
   // myMap.location(move_index, latitude, longitude, "GPS_Location");
    //speed = gps.speed.kmph();               //get speed


    direction = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
    
    satellites = gps.satellites.value();    //get number of satellites
 


  }


  //Serial.println();
}
