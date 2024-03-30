#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
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

// Your GPRS credentials, if any
const char apn[] = "inernet.ng.airtel.com"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "web";
const char gprsPass[] = "web";


// MQTT details
const char* broker = "broker.hivemq.com";                    // Public IP address or domain name
const char* mqttUsername = "REPLACE_WITH_YOUR_MQTT_USER";  // MQTT username
const char* mqttPassword = "REPLACE_WITH_YOUR_MQTT_PASS";  // MQTT password

const char* topicinbound = "projx/inbound";
const char* topicoutbound = "projx/outbound";

//Pin for ESP32
  #define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         8 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       14 //Clock = SCK connect to pin 18

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

#define BL_PIN 9  // Backlight pin, adjust this according to your wiring
#define SCREEN_GND_PIN 26  // Ground pin for screen
#define RFID_GND_PIN 27  // Ground pin for RFID


#define DEBUG true

unsigned long lastActivityTime = 0;
bool screenActive = true;
#define SCREEN_TIMEOUT 300000 // 5 minutes in milliseconds


const uint8_t KEYPAD_ADDRESS = 0x38;

I2CKeyPad keyPad(KEYPAD_ADDRESS);

uint32_t start, stop;
uint32_t lastKeyPressed = 0;


int first = 0;
int counter = 0;
String uID;


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
SoftwareSerial serial1(0, 1); // RX, TX

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);





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
