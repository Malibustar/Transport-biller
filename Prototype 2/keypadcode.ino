//
//    FILE: I2Ckeypad_5x3.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo 5x3 or smaller keypad
//     URL: https://github.com/RobTillaart/I2CKeyPad
//
//  PCF8574
//    pin p0-p4  rows
//    pin p5-p7  columns


#include "Wire.h"



void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }

  keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
}


void loop()
{
  uint32_t now = millis();
  //  adjust keymap if needed
  char keys[] = "1234567890ABCDE NF";  //  N = NoKey, F = Fail

  if (now - lastKeyPressed >= 100)
  {
    lastKeyPressed = now;

    start = micros();
    uint8_t index = keyPad.getKey();
    stop = micros();

    Serial.print(millis());
    Serial.print("\t");
    Serial.print(index);
    Serial.print("\t");
    Serial.print(keys[index]);
    Serial.print("\t");
    Serial.println(stop - start);
  }
}


//  -- END OF FILE --








/*
  SerialPassthrough sketch

  Some boards, like the Arduino 101, the MKR1000, Zero, or the Micro, have one
  hardware serial port attached to Digital pins 0-1, and a separate USB serial
  port attached to the IDE Serial Monitor. This means that the "serial
  passthrough" which is possible with the Arduino UNO (commonly used to interact
  with devices/shields that require configuration via serial AT commands) will
  not work by default.

  This sketch allows you to emulate the serial passthrough behaviour. Any text
  you type in the IDE Serial monitor will be written out to the serial port on
  Digital pins 0 and 1, and vice-versa.

  On the 101, MKR1000, Zero, and Micro, "Serial" refers to the USB Serial port
  attached to the Serial Monitor, and "Serial1" refers to the hardware serial
  port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  created 23 May 2016
  by Erik Nyquist

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/SerialPassthrough
*/

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial2.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial2.read());   // read it and send it out Serial (USB)
  }
}