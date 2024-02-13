#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include "Wire.h"
#include "I2CKeyPad.h"


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

#define BL_PIN 9  // Backlight pin, adjust this according to your wiring
#define SCREEN_GND_PIN 26  // Ground pin for screen
#define RFID_GND_PIN 27  // Ground pin for RFID


unsigned long lastActivityTime = 0;
bool screenActive = true;
#define SCREEN_TIMEOUT 300000 // 5 minutes in milliseconds



int first = 0;
int counter = 0;
String uID;


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.


// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);




void setup() {
  Serial.begin(115200);
 // Set console baud rate
  delay(10);
   Serial1.begin(115200, SERIAL_8N1, D0, D1); //for A9G Board

  
  pinMode(BL_PIN, OUTPUT);
  pinMode(SCREEN_GND_PIN, OUTPUT);
  pinMode(RFID_GND_PIN, OUTPUT);
  //digitalWrite(BL_PIN, HIGH);  // Turn on the backlight initially

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  Serial.println("Screen Initialized");


tft.fillScreen(ST77XX_BLACK);
int w = tft.width();
int h = tft.height();
Serial.print("width =");
Serial.println(w);
Serial.print("height =");
Serial.println(h);
//expecting 128 x 160
 tft.setRotation(1); 


  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("Booting..."); // display "scan now" on device screen
  Serial.println("Booting..."); 

//AT + CSQ // get radio signal strength
//AT + CBC //get battery charge leve and battery charging status

 serial.write("AT+CGATT=1", 3000); // Connect to the network
    serial.write("AT+CGDCONT=1,\"IP\",\"apn\"", 1000);    
    serial.write("AT+CGACT=1,1", 1000);
    delay(500);



    serial.write("AT+MQTTCONN=\"test.mosquitto.org\",1883,\"mqttx_0931852d34\",120,0", 1000);
    delay(100);
    sendData("AT+MQTTSUB=\"/public/TEST/makerfabs-B\",1,0", 1000);
    
    //sendData("AT+CIPSTART=\"TCP\",\"www.mirocast.com\",80", 2000, DEBUG);
    serial.write("AT+GPS=1",3000); //turn on GPS MODULE
    serial.write("AT+GPSRD=0", 1000); //stop sending GPS data
   // serial.write("AT+GPS=0");

Serial1.println("AT+GPSLP = 2") //GPS Low power


SerialMon.println("Wait...");



  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");

tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(15, 0); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Signal strength " ) + serial.write("AT + CSQ")"%" ); // display "menu" on device screen
  // Serial.println(modem.getSignalQuality() );

delay(1000);
}
//end of void setup
void loop() {
  
// checkNetwork();



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
 
/*
   tft.setCursor(5, 0); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Signal strength " + String(modem.getSignalQuality()) + "%" ); // display "menu" on device screen
  // Serial.println(modem.getSignalQuality() );
*/

//if (checkcard()==true && checkNetwork()==true) {
if (checkcard()==true) {
  String deviceinfo = amountString + "," + uID;
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing payment.."); // display processing on device screen
tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing successful.."); // display processing on device screen
 delay(1000);
tft.fillScreen(ST77XX_BLACK);
Serial.println("mqtt2 done");
Serial.print(deviceinfo);


}


/*
String deviceinfo = amountString + "," + uID;
mqtt.publish(topicoutbound, deviceinfo.c_str());
Serial.print(deviceinfo);
mqtt.loop();*/



}

bool checkcard()
{
 // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  //Serial.print("Card UID:");



  int val1 = (mfrc522.uid.uidByte[0]);
  int val2 = (mfrc522.uid.uidByte[1]);
  int val3 = (mfrc522.uid.uidByte[2]);
  int val4 = (mfrc522.uid.uidByte[3]);

  String valA = String(val1);
  String valB = String(val2);
  String valC = String(val3);
  String valD = String(val4);
  uID = valA + valB + valC + valD;
  Serial.print(uID); //Display card UID

  digitalWrite(BL_PIN, HIGH);
tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(2);
tft.print("Processing...."); // display processing on device screen
delay(500);
digitalWrite(BL_PIN, LOW);
tft.fillScreen(ST77XX_BLACK);
Serial.println();

enterAmount(); // calls enter amount function for user to input amount

  
  counter = counter + 1;
  Serial.print(counter);

  //}
  Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  if (counter > first)
  { //delay ifs
    // enable w5100 SPI

 
      first++;
  }
  return true;
  
}

void readKeyPad()
{
  
}

bool checkNetwork() {

    //Reconnect gsm
    if (!modem.isNetworkConnected())
    {
        Serial.println(("Network disconnected"));
        if (!modem.waitForNetwork(1000L, true))
        {
            count++;
            if (count > 15)
            {
                count = 0;
                Serial.println("rst");
                modem.restart();
            }
        }
    }

        if ( modem.isNetworkConnected() && modem.getSignalQuality() >= 16 && !modem.isGprsConnected())
    {
        
        Serial.println("connecting to internet");
        
        if (!modem.gprsConnect(apn, gprsUser, gprsPass))
        {
        }
    }
    

    //Reconnect mqtt
    if (modem.isGprsConnected() && modem.getSignalQuality() >= 16 && !mqtt.connected())
    {
        
        Serial.println("CONTACTING SERVER");

        
        count++;
        Serial.println(count);
        if (count > 250)
        {
            count = 0;
            modem.restart();
            delay(100);
        }
        if (!mqttConnect())
        {
            // lastReconnectAttempt = 0;
        }
     
}
     if (mqtt.connected() )
    {
        mqtt.loop();
       return true;
    }
    else{
      return false;
    }
}
void enterAmount() {

  Serial.println("Input Amount:");

  tft.setCursor(30, 45); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Enter Amount:"); // display "Enter Amount:" on device screen

  amountString = ""; // Clear the previous amount

  while (true) {
    readKeyPad(); // Continue reading keypad to capture amount input

if (stringAdded) { // display amount pressed on device screen
    stringAdded = false;
    if (pressedCharacter != "C") {
      amountString += pressedCharacter;
      Serial.println("Enter Amount: " + amountString);

  tft.setCursor(10, 65); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("N:" + amountString); // display "Enter Amount:" on device screen

    }
  }

    // Check if the "C" key is pressed to exit the function
    if (buttonPressed && pressedCharacter == "C") {
      buttonPressed = false; // Reset buttonPressed to avoid immediate triggering
      Serial.println("Amount Entered");
      tft.fillScreen(ST77XX_BLACK);
      return; // Exit the function
    }

 
  }
}
