
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>



//Pin for ESP32
  #define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18

#define SS_PIN 5  
#define RST_PIN 0


float buttonTimer = 0;
bool buttonPressed = false;
String pressedCharacter = "";
bool stringAdded = false;
String amountString = "";

#define BL_PIN 4  // Backlight pin, adjust this according to your wiring

unsigned long lastActivityTime = 0;
bool screenActive = true;
#define SCREEN_TIMEOUT 300000 // 5 minutes in milliseconds



int first = 0;
int counter = 0;
String uID;


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.


// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

  
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

void setup() {
  Serial.begin(115200);
 // Set console baud rate
  //mySerial.begin(9600);
  
  pinMode(BL_PIN, OUTPUT);
  //digitalWrite(BL_PIN, HIGH);  // Turn on the backlight initially

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  Serial.println("Screen Initialized");

 Wire.begin();
  Wire.setClock(400000L);


tft.fillScreen(ST77XX_BLACK);
int w = tft.width();
int h = tft.height();
Serial.print("width =");
Serial.println(w);
Serial.print("height =");
Serial.println(h);
//expecting 128 x 160


  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("Booting..."); // display "scan now" on device screen
  Serial.println("Booting..."); 

delay(5000);

 SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");

tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 

delay(2000);

}//end of void setup
void loop() {
  

  //draw a rectangle

  tft.setCursor(32, 50); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("SCAN  TO PAY"); // display "scan now" on device screen
 // tft.drawRect(0,100, 0, 100, ST77XX_YELLOW);

  Serial.println("SCAN TO PAY"); 

 //tft.drawLine(0, 100, 0, 100, ST77XX_YELLOW);


  tft.setCursor(52, 150); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Menu"); // display "menu" on device screen
 

checkcard(); // Waiting for user to scan card


}

void checkcard()
{
 // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
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
delay(1000);
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
        pressedCharacter = "1";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101101 - P1 pin went low after pressing the button -> 2 was pressed
      case 237: //Button 2
        Serial.print("2");
        pressedCharacter = "2";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101011 - P2 pin went low after pressing the button -> 3 was pressed
      case 235: //Button 3
        Serial.print("3");
        pressedCharacter = "3";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11100111 - P3 pin went low after pressing the button -> A was pressed
      case 231: //Button A
        Serial.println("A");
        pressedCharacter = "A";
        buttonPressed = true;
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
        Serial.print("4");
        pressedCharacter = "4";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 221: //Button 5
        Serial.print("5");
        pressedCharacter = "5";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 219: //Button 6
        Serial.print("6");
        pressedCharacter = "6";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 215: //Button B
        Serial.println("B");
        pressedCharacter = "B";
        buttonPressed = true;
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
        Serial.print("7");
        pressedCharacter = "7";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 189: //Button 8
        Serial.print("8");
        pressedCharacter = "8";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 187: //Button 9
        Serial.print("9");
        pressedCharacter = "9";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 183: //Button C
        Serial.println("C");
        pressedCharacter = "C";
        buttonPressed = true;
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
        Serial.print(".");
        pressedCharacter = ".";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 125: //Button 0
        Serial.print("0");
        pressedCharacter = "0";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 123: //Button #
        Serial.print("#");
        pressedCharacter = "#";
        buttonPressed = true;
        break;

      case 119: //Button D
        Serial.println("D");
        pressedCharacter = "D";
        buttonPressed = true;
        break;
    }
    buttonTimer = millis();
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