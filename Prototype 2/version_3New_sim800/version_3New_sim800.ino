#define DEBUG true
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include "Wire.h"
#include "I2CKeyPad.h"
#include<String.h>
#define DEBUG true

#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define Serial Serial

// or Software Serial on Uno, Nano

#include <SoftwareSerial.h>
SoftwareSerial mySerial(27, 26);  // RX, TX


#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

#define TINY_GSM_DEBUG Serial

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "internet.gprs.airtel.com";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details
const int  port       = 80;
const char* serverName = "ondigo.us-east-1.elasticbeanstalk.com";



#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(mySerial, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(mySerial);
#endif

TinyGsmClient client(modem);
HttpClient   http(client, serverName, port);
//const byte rxPin = 27;
//const byte txPin = 26;
  

#include <Wire.h>

void displayMenu();
void displaySubitems();
void displayPage();
void goBack();
void selectItem();
void scrollDown();
void scrollUp();
void showmenu(void);


  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18



#define SS_PIN 5  
#define RST_PIN 4


long lastMsg = 0;
int count = 0;
float buttonTimer = 0;
bool buttonPressed = false;
String pressedCharacter = "";
bool stringAdded = false;
String amountString = "";
unsigned long elapsedTime = 0;
unsigned long previousMillis = 0;
String paymentinfo;
String msg;


 int febucheck;
int day=00;int month=00;int year=00;int hour=00;int mint=00;int seconds;

String machineID = "ondigo000000000";


 int batteryLevel, signalStrength;
String deviceID = "ondigo12345" ;
String tfare, walletBal, admin;
 String DEVICEID, RESPONSE;

#define BL_PIN 33  // Backlight pin, adjust this according to your wiring
#define Buzzer 14
 //******************* MQTT Parameters *******************//




//******************* SIM Paramaters *******************//


//******************* Necessary Variables *******************//
char* response = " ";
int counter = 0;
String uID;

int progress = 0;
int gettime = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
SoftwareSerial serial1(0, 1); // RX, TX

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


extern uint8_t logo_round_rect[];
extern uint8_t  go_logo[];
extern uint8_t  battery_17[];
extern uint8_t  battery_33[];
extern uint8_t  battery_50[];
extern uint8_t  battery_67[];
extern uint8_t  battery_83[];
extern uint8_t  battery_full[];
extern uint8_t  charger_connected[];
extern uint8_t  network_1_bar[];
extern uint8_t  network_2_bars[];
extern uint8_t  network_3_bars[];
extern uint8_t  network_4_bars[];
extern uint8_t  network_not_connected[];
extern uint8_t  internet[];
extern uint8_t location_icon[];

void setup(void) {


  
  pinMode(BL_PIN, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  delay(100);
 digitalWrite(BL_PIN, HIGH); 

 
   Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200);
  Serial2.begin(115200);
  SPI.begin();      // Init SPI bus
  Serial.println("ILI9341 Test!");
  mfrc522.PCD_Init();        // Init MFRC522 card 
  

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
   mySerial.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif


    //setupGPRS();

    // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  tft.setRotation(1);
  tft.invertDisplay(false);

 tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame


tft.drawBitmap(127, 89, logo_round_rect, 63, 49, ST77XX_WHITE);
tft.drawBitmap(132, 95, go_logo, 51, 36, ST77XX_WHITE);
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(2);
tft.setCursor(55, 145);
tft.print("powered by ONDIGO");

for (int progress = 0; progress <= 171; progress++){  // you can change the increment value here
    //tft.drawRect(x, y, w, h, colour);
tft.drawRect(71, 201, 171, 2, ST77XX_WHITE);
tft.drawRect(71, 201, progress, 2, ST77XX_GREEN);

    delay(100);  //change the delay to change how fast the boot up screen changes 
  }



 tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame

gettime = 0;
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




bool attemptCommand(String command, int timeout, int attempts, String successIndicator) {
  for (int tryCount = 0; tryCount < attempts; tryCount++) {
    String response = sendData(command, timeout, DEBUG);
    if (response.indexOf(successIndicator) != -1) {
      return true; // Success
    }
    delay(1000); // Wait a bit before retrying
  }
  Serial.println("Failed to execute command: " + command);
  return false; // Failed after all attempts
}


String sendData(String command, const int timeout, boolean debug)
{
  String temp = "";
  Serial2.println(command);
  delay(500);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
      temp += c;
    }
  }
  if (debug)
  {
    Serial.print(temp);
  }
  return temp;
}



bool checkSIMStatus() {
    String cpinResponse = sendData("AT+CPIN?", 5000, DEBUG);
    if (cpinResponse.indexOf("+CPIN: READY") != -1) {
        Serial.println("SIM is ready.");
        return true;
    } else {
        Serial.println("SIM not ready: " + cpinResponse);
        return false;
    }
}


void refreshtimescreen(){

tft.setTextColor(ST77XX_BLACK);
tft.setTextSize(6);
tft.setTextWrap(false);
tft.setCursor(70, 100);
tft.print(hour + 1); 
tft.setCursor(140, 100);
tft.print(":"); 
tft.setCursor(175, 100);
tft.print(mint); 
tft.setTextSize(2);
tft.setTextWrap(false);
tft.setCursor(90, 155);
tft.print(day); //"13:03:2024"
tft.setCursor(145, 155);
tft.print(month);
tft.setCursor(195, 155);
tft.print(year);

}

void nettime(){

  Serial.print(hour);
  Serial.print(":");
  Serial.print(mint);
  delay(990);  
  seconds++; 
  Serial.print(":");
  Serial.print(seconds);
  if(seconds==60)
    {
      seconds=0;
 // tft.fillScreen(ST77XX_BLACK);
 //tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
  refreshtimescreen();
      mint++;
          
  }
  if(mint==60)
    {
      mint=0;
      refreshtimescreen();
      hour++;
      
    }
    if((month==1 || month==3 || month==5 || month==7 || month==8 || month==10 )&& day==31 && 
    hour==24)
    {
      day=1;
      seconds=0;
      mint=0;
      hour=0;
      refreshtimescreen();
      month++;
      
    
    }
    if(month==12 && day==31 && hour==24)
    {
      month=1;
      day=1;
      seconds=0;
      mint=0;
      hour=0;
      refreshtimescreen();
      year++;
      
      if(year>2000)
      febucheck=year-2000;
      else if(year<2000)
      febucheck=2000-year;
      else
      febucheck=0;
    }
    if(month==2 && (febucheck%4)==0 && day==29 && hour==24)
    {
      day=1;
      seconds=0;
      mint=0;
      hour=0;
      refreshtimescreen();
      month++; 
     
    }
    else if(month==2 && (febucheck%4)!=0 && day==28 && hour==24)
    {
      day=1;
      seconds=0;
      mint=0;
      hour=0;
      refreshtimescreen();
      month++;
      
    }
    else{}
    if(hour==24)
    {
      hour=0;
       refreshtimescreen();
      day++;
    
    }
  Serial.print(" Date:");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);



 
  }

void setTimeFromGSM() {

    while (Serial2.available()) {
    Serial2.println("AT+CCLK?");
    delay(1000); // Wait for response
    String response = Serial2.readStringUntil('\n');
    Serial.println("Received response: " + response);
/*
     String response = "";
   
// Send AT+CBC? command and wait for response
     response = sendData("AT+CCLK?", 1000, DEBUG);

  // Check if the response starts with "+CCLK:" and has the minimum required length
    if (response.startsWith("AT+CCLK?") && response.length() > 18) {

*/
    // Check if the response starts with "+CCLK:" and has the minimum required length
    if (response.startsWith("+CCLK:") && response.length() > 18) {
      // Extract date and time components manually
      String dateStr = response.substring(8, 20);
      String timeStr = response.substring(17, 22);

      // Parse date components
      day = dateStr.substring(0, 2).toInt();
      month = dateStr.substring(3, 5).toInt();
      year = dateStr.substring(6, 8).toInt();

      // Parse time components
      hour = timeStr.substring(0, 2).toInt();
      mint = timeStr.substring(3, 5).toInt();
      seconds = timeStr.substring(6, 8).toInt();
/*
      // Print parsed values for debugging
      Serial.print("Parsed date: ");
      Serial.print(day);
      Serial.print("/");
      Serial.print(month);
      Serial.print("/");
      Serial.println(year);
      Serial.print("Parsed time: ");
      Serial.print(hour);
      Serial.print(":");
      Serial.print(mint);
      Serial.print(":");
      Serial.println(seconds);*/

      // Exit the loop after successfully setting the time
      gettime = 1;
      Serial.println("Time set successfully.");
      break;
    } else {
      Serial.println("Invalid response format.");
    }
  }
}


void readKeyPad()
{
  if (buttonPressed == true)
  {
    if (millis() - buttonTimer > 300)
    {
      //The button pressed is only set back to false after 300 ms, so we cannot press a button twice quickly
      buttonTimer = millis();
      buttonPressed = false;
    }
    else
    {
      //do nothing
    }
  }
  else
  {
    //B11101111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B11101111);
    //[P7]B11101111[P0] -> [P7]1110[P4] - activates first row, [P3]1111[P0] - Sets all pins high on the MUX
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      //11101110 - P0 pin went low after pressing the button -> 1 was pressed
      case 238: //Button 1
        Serial.print("1");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "1";
        break;

      //11101101 - P1 pin went low after pressing the button -> 2 was pressed
      case 237: //Button 2
        Serial.print("4");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "4";
        break;

      //11101011 - P2 pin went low after pressing the button -> 3 was pressed
      case 235: //Button 3
        Serial.print("7");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "7";
        break;

      //11100111 - P3 pin went low after pressing the button -> A was pressed
      case 231: //Button A
        Serial.println("*");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true; 
        pressedCharacter = "*";
        break;
    }
    //-------------------------------------------
    //B11011111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B11011111);
    //[P7]B11011111[P0] -> [P7]1101[P4] - activates second row, [P3]1111[P0] - Sets all pins high on the MUX
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      //11011110 - P0 pin went low after pressing the button -> 2 was pressed
      case 222: //Button 4
        Serial.print("2");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "2";
        break;

      case 221: //Button 5
        Serial.print("5");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "5";
        break;

      case 219: //Button 6
        Serial.print("8");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "8";
        break;

      case 215: //Button B
        Serial.println("0");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "0";
        break;
    }
    //-------------------------------------------
    //B10111111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B10111111);
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      case 190: //Button 7
        Serial.print("3");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "3";
        break;

      case 189: //Button 8
        Serial.print("6");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "6";
        break;

      case 187: //Button 9
        Serial.print("9");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        stringAdded = true;
        pressedCharacter = "9";
        break;

      case 183: //Button C
        Serial.println("#");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        pressedCharacter = "#";
        break;
    }
    //-------------------------------------------
    //B01111111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B01111111);
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      case 126: //Button *
        Serial.print("A");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        pressedCharacter = "A";
        break;

      case 125: //Button 0
        Serial.print("B");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        pressedCharacter = "B";
        break;

      case 123: //Button #
        Serial.print("C");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        buttonPressed = true;
        pressedCharacter = "C";
        break;

      case 119: //Button D
        Serial.println("D");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
         buttonPressed = true;
        pressedCharacter = "D";     
        break;
    }
    buttonTimer = millis();
  }
}

void enterAmount() {
 
tft.fillScreen(ST77XX_BLACK);
tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
  Serial.println("Input Amount:");

tft.setTextColor(0xEDC0);
tft.setTextSize(2);
tft.setCursor(85, 70);
tft.print("Enter Amount:");
tft.setTextSize(3);
tft.setCursor(124, 200);
tft.print("Back");

  amountString = ""; // Clear the previous amount

  while (true) {

    readKeyPad(); // Continue reading keypad to capture amount input

if (stringAdded) { // display amount pressed on device screen
    stringAdded = false;
    if (pressedCharacter != "#") {
      amountString += pressedCharacter;
      Serial.println("Enter Amount: " + amountString);
       }

     tft.setTextColor(ST77XX_WHITE);
     tft.setTextSize(4);
     tft.setCursor(65, 120);
     tft.print("N:");
      tft.setCursor(115, 120);
     tft.print(amountString);
   
  }

 if (pressedCharacter == "C" ) { // clear field when "C" pressed
     tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(115, 120);
     tft.print(amountString);
     amountString = "";
     
    }
      
    // Check if the "#" key is pressed to exit the function
    if (buttonPressed && pressedCharacter == "#") {
      Serial.println("Amount Entered");
  
}


 if (pressedCharacter == "B" ) { // Delete on "C" press if string has characters
        amountString = "";
      tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
  break; // Exit the function
      }
 
  }
}

void Processpayment(){
 if (checkCard()==true){

 //uID = "193596173";
   
     paymentinfo = amountString + "," + uID;
      //String payload = "{\"rfid\":\"" + uID + "\", \"amount\":\"" + amountString + "\"}";
      tft.fillScreen(ST77XX_BLACK);
      tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
      tft.setCursor(0, 57); 
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(3);
      tft.print("Processing payment.."); // display processing on device screen
      Serial.print("payment info - "); Serial.print(paymentinfo); Serial.println("-");

     // Serializing in JSON Format
  DynamicJsonDocument docc(1024);
  docc["UID"] = uID;
  docc["amount"] = amountString;
  docc["machineId"] = machineID;
  serializeJson(docc, msg);
  Serial.println(msg);

  connectapnandinternet();
  Paymentserverrequest();

Serial.println("Payment Successful.."); // display successful serial monitor
delay(3000);

   break; // Exit the function
      }
      

}

void connectapnandinternet(){
  // GPRS connection parameters are usually set after network registration
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
  tft.fillScreen(ST77XX_BLACK);
tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(3);
tft.print("Connection failed.."); // display processing on device screen
    delay(10000);
    return;
  }
  Serial.println(" success");

  if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }



}


void Paymentserverrequest() {

Serial.print(F("Performing HTTPS GET request... "));
 
  String contentType = "application/json"; // "application/x-www-form-urlencoded"; 
  const char resource[] = "/payment";
  http.connectionKeepAlive();  // Currently, this is needed for HTTPS

   //http.sendHeader("Transfer-Encoding", "Chunked"); 
   int err = http.post(resource, contentType, msg);
   
        
  if (err != 0) {
    Serial.println(F("failed to connect"));
    Serial.println(err);
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  Serial.print(F("Response status code: "));
  Serial.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  Serial.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName  = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    Serial.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    Serial.print(F("Content length is: "));
    Serial.println(length);
  }
  if (http.isResponseChunked()) {
    Serial.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  Serial.println(F("Response:"));
  Serial.println(body);

  Serial.print(F("Body length is: "));
  Serial.println(body.length());

  // Shutdown

  http.stop();
  Serial.println(F("Server disconnected"));


  modem.gprsDisconnect();
  Serial.println(F("GPRS disconnected"));
  
}


bool checkCard() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  // Standard key for MIFARE authentication
  }

  tft.fillScreen(ST77XX_BLACK); // Clear the screen
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Please present your card");

  unsigned long startTime = millis();
  while (true) {
    // Check if a new card is present
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Card detected!");

      // Construct the card UID string
      uID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uID += String(mfrc522.uid.uidByte[i], HEX);
      }
      uID.toUpperCase();

      Serial.print("Card UID: ");
      Serial.println(uID);

      // Halt PICC and stop encryption on PCD
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return true;  // Card read successfully
    }

    // Check for timeout to prevent infinite looping
    if (millis() - startTime > 10000) {  // 10 seconds timeout
      Serial.println("Timeout waiting for card.");
      tft.fillScreen(ST77XX_BLACK); // Clear the screen
      tft.setCursor(10, 10);
      tft.println("Timeout, try again.");
      delay(2000); // Show message for 2 seconds
      return false;
    }

    delay(50);  // Small delay to prevent busy looping
  }
}

void getBatteryState(){
/*
tft.drawBitmap(275, 147, charger_connected, 24, 16, ST77XX_WHITE);
tft.drawBitmap(19, 86, network_1_bar, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 61, network_2_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 38, network_3_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 110, network_not_connected, 15, 16, ST77XX_WHITE);
tft.drawBitmap(48, 15, internet, 15, 16, ST77XX_WHITE);
tft.drawBitmap(79, 14,location_icon, 13, 16, ST77XX_WHITE);
*/

String response = "";
static int prevChargerState = -1; // Store the previous state of chargerState, initialized to an invalid value


// Send AT+CBC? command and wait for response
 response = sendData("AT+CBC?", 1000, DEBUG);


  // Check if the response starts with "+CCLK:" and has the minimum required length
    if (response.startsWith("AT+CBC?") && response.length() > 6) {
      // Extract battery components manually
      int chargerState = response.substring(17, 18).toInt();
      int batteryPercentage = response.substring(20, 23).toInt();;

        // Parse the substring
        Serial.print("Charging state: ");
        Serial.println(chargerState == 1 ? "Connected" : "Not connected");

       // Check if chargerState has changed before executing the if statement
        if (chargerState != prevChargerState) {
            // Update prevChargerState
            prevChargerState = chargerState;

        // Check chargerState value and draw bitmap accordingly
        if (chargerState == 1) {
          tft.drawBitmap(275, 13, battery_full, 24, 16, ST77XX_BLACK);
          tft.drawBitmap(275, 13, charger_connected, 24, 16, ST77XX_BLACK);
            tft.drawBitmap(275, 13, charger_connected, 24, 16, ST77XX_WHITE);
        } 
        
        else {
          tft.drawBitmap(275, 13, charger_connected, 24, 16, ST77XX_BLACK);
          tft.drawBitmap(275, 13, battery_full, 24, 16, ST77XX_BLACK);
            // Map battery percentage to 6 stages
            int batteryStage = map(batteryPercentage, 0, 100, 0, 5);
            Serial.print("Battery stage: ");
            switch (batteryStage) {
                case 0:
                    Serial.println("0-16%");
                    tft.drawBitmap(275, 13, battery_17, 24, 16, ST77XX_WHITE);
                    break;
                case 1:
                    Serial.println("17-33%");
                    tft.drawBitmap(275, 13, battery_33, 24, 16, ST77XX_WHITE);
                    break;
                case 2:
                    Serial.println("34-49%");
                    tft.drawBitmap(275, 13, battery_50, 24, 16, ST77XX_WHITE);
                    break;
                case 3:
                    Serial.println("50-66%");
                    tft.drawBitmap(275, 13, battery_67, 24, 16, ST77XX_WHITE);
                    break;
                case 4:
                    Serial.println("67-83%");
                    tft.drawBitmap(275, 13, battery_full, 24, 16, ST77XX_BLACK);
                    tft.drawBitmap(275, 13, battery_83, 24, 16, ST77XX_WHITE);
                    break;
                case 5:
                    Serial.println("84-100%");
                    tft.drawBitmap(275, 17, battery_full, 24, 16, ST77XX_WHITE);
                    break;
                default:
                    Serial.println("Unknown");
            }
        }
        }
    }
     else {
        Serial.println("Failed to parse response.");
    }
} 

void drawhomepage(){
//tft.fillRoundRect(0, 56, 400, 48, 1, ST77XX_BLACK);
//tft.setTextColor(ST77XX_BLACK);
tft.setTextColor(0xFFFF);
tft.setTextSize(6);
tft.setTextWrap(false);
tft.setCursor(70, 100);
tft.print(hour + 1); 
tft.setCursor(140, 100);
tft.print(":"); 
tft.setCursor(175, 100);
tft.print(mint); 
tft.setTextSize(2);
tft.setTextWrap(false);
tft.setCursor(90, 155);
tft.print(day); //"13:03:2024"
tft.setCursor(145, 155);
tft.print(month);
tft.setCursor(195, 155);
tft.print(year);
//tft.print(day + ":" + month +":"+ "20"+ year); //"13:03:2024"
tft.setTextColor(0xEDC0);
tft.setTextSize(3);
tft.setCursor(124, 200);
tft.print("Menu");
tft.drawBitmap(79, 16,location_icon, 13, 16, ST77XX_WHITE);
tft.drawBitmap(136, 54, go_logo, 51, 36, 0xF5A0);
//tft.drawBitmap(48, 16, internet, 15, 16, ST77XX_WHITE);
getBatteryState();
getNetworkState();

tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(275, 13, battery_83, 24, 16, ST77XX_WHITE);

}

void checkiftimeisregistered(){

//enterAmount(); // calls enter amount function for user to input amount

if(gettime==0){
  // Initialize time
  setTimeFromGSM(); // Get time from GSM module
   }

if (gettime==1){
nettime();

}

}

void getNetworkState(){

   String response = "";
   
// Send AT+CBC? command and wait for response
     response = sendData("AT+CSQ", 1000, DEBUG);


  // Check if the response starts with "+CCLK:" and has the minimum required length
    if (response.startsWith("AT+CSQ") && response.length() > 6) {

      // Network components manually

      String NetworkStrength = response.substring(16, 18);
      String NetworkSNR = response.substring(19, 22);

      Serial.println("NetworkStrength:" + NetworkStrength);
      Serial.println("NetworkSNR:" + NetworkSNR);

        // Convert NetworkStrength to an integer
        int networkStrengthInt = NetworkStrength.toInt();
Serial.println("NetworkStrengthint:" + networkStrengthInt);


            // Print charging state
            Serial.print("NetworkStrength: ");
            Serial.println(networkStrengthInt <= 1 ? "network available" : "No network");


    
            // Check chargerState value and draw bitmap accordingly
            if (networkStrengthInt <= 0) {
              tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_BLACK);
              tft.drawBitmap(19, 16, network_not_connected, 15, 16, ST77XX_BLACK);
                tft.drawBitmap(19, 16, network_not_connected, 15, 16, ST77XX_WHITE);
            } else {
              tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_BLACK);
              tft.drawBitmap(19, 16, network_not_connected, 15, 16, ST77XX_BLACK);

              
                // Map battery percentage to 6 stages
                int networkStage = map(networkStrengthInt, 0, 100, 0, 5);
                Serial.print("Network stage: ");
                switch (networkStage) {
                    case 0:
                        Serial.println("0-16%");
                        tft.drawBitmap(19, 16, network_1_bar, 15, 16, ST77XX_WHITE);
                        break;
                    case 1:
                        Serial.println("17-33%");
                        tft.drawBitmap(19, 16, network_2_bars, 15, 16, ST77XX_WHITE);
                        break;
                    case 2:
                        Serial.println("34-49%");
                        tft.drawBitmap(19, 16, network_3_bars, 15, 16, ST77XX_WHITE);
                        break;
                    case 3:
                        Serial.println("50-66%");
                        tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
                        break;
          
                    default:
                        Serial.println("Unknown");
                }
            }
        } else {
            Serial.println("Failed to parse response.");
        }
    }
 
void checktodisplaymenu(){
if (buttonPressed == true && pressedCharacter == "A")
  {
    displayMenu();
    while (true) {
    showmenu() ;
   if (buttonPressed == true && pressedCharacter == "B" ) { // Move to home page
    tft.fillScreen(ST77XX_BLACK);
    tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
    break; // Exit the function
      }
    }
  }
}

void checktodisplayamount(){
  if (buttonPressed == true&& pressedCharacter == "#") //if a button was pressed...
  {
  enterAmount();
  }
}

void resetclockandgetnewtime(){
 // Check if 300 seconds (5 minutes) have elapsed
  if (elapsedTime >= 300000) { // 300000 milliseconds = 300 seconds
    Serial.println("reset network time");
    gettime==0;
    elapsedTime = 0; // Reset the timer
}
}

void loop() {
   
uID = "";
amountString = "";

//check if the module has success
checkiftimeisregistered();
//display the home page of the device
drawhomepage();
//check if the menu button is pressed to display menu
checktodisplaymenu();
//read the keypad
readKeyPad(); 
//check if the # button is pressed to display amount screen
checktodisplayamount();
Processpayment();
  // Update elapsed time
  elapsedTime += millis() - previousMillis;
  previousMillis = millis();
// Check if 300 seconds (5 minutes) have elapsed, reset time variable
 resetclockandgetnewtime();
  
/*
  Serial.print("Year: ");
  Serial.println(year);
  Serial.print("Month: ");
  Serial.println(month);
  Serial.print("Day: ");
  Serial.println(day);
  Serial.print("Hour: ");
  Serial.println(hour);
  Serial.print("Minute: ");
  Serial.println(minute);
  Serial.print("Second: ");
  Serial.println(second);
  */
  }



