#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include "Wire.h"
#include "I2CKeyPad.h"



void setup();
void readKeyPad();
void EnterAmount();
void Getsignalandbattery();
void RecieveMessage();
bool checkcard();
void sendmoney();


#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18

#elif defined(ESP8266)
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18

#endif



#define SS_PIN 5  
#define RST_PIN 4


long lastMsg = 0;
int count = 0;
float buttonTimer = 0;
bool buttonPressed = false;
String pressedCharacter = "";
bool stringAdded = false;
String amountString = "";
String sub_message ="";



String machineserial = "ondigo000000000";


 int batteryLevel, signalStrength;
String deviceID, tfare, walletBal, admin;
 String SERDEVICEID, RESPONSE;

#define BL_PIN 33  // Backlight pin, adjust this according to your wiring
#define SCREEN_GND_PIN 26  // Ground pin for screen
#define RFID_GND_PIN 27  // Ground pin for RFID




int first = 0;
int counter = 0;
String uID;

int progress = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
SoftwareSerial serial1(0, 1); // RX, TX

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);




void loop() {
  
uID = "";
amountString = "";

enterAmount(); // calls enter amount function for user to input amount

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(20, 50); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("SCAN TO PAY"); // display "scan now" on device screen
 // tft.drawRect(0,100, 0, 100, ST77XX_YELLOW);

  Serial.println("SCAN TO PAY"); 

 //tft.drawLine(0, 100, 0, 100, ST77XX_YELLOW);


  tft.setCursor(70, 120); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Menu"); // display "menu" on device screen

 
checkcard(); //This function checks if a card was scanned

//if (checkcard()==true && checkNetwork()==true) {
if (checkcard()==true) {
  String deviceinfo = amountString + "," + uID;
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing payment.."); // display processing on device screen


sendMoney(); //This function is called whenever you want to request for payment 


}




}


void drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
    }
  }
}