//******* Pin Configurations *******//
/
#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(27, 26);  // RX, TX


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);// For ESP32
  mySerial.begin(115200); //For A9G



}

void loop()
{
  // read from port 1, send to port 0:
  if (mySerial.available()) {
    int inByte = mySerial.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    mySerial.write(inByte);
  }
}
