
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

//Pin for ESP32
  #define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18

#define SS_PIN 5  
#define RST_PIN 0




//#define BL_PIN 3 5  // Backlight pin, adjust this according to your wiring

unsigned long lastActivityTime = 0;
bool screenActive = true;
#define SCREEN_TIMEOUT 300000 // 5 minutes in milliseconds

// Define the keypad matrix
const byte ROW_NUM    = 4; // four rows
const byte COLUMN_NUM = 4; // four columns

// Define the key mapping
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte pin_rows[ROW_NUM] = {27, 14, 21, 33};

// Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.
byte pin_column[COLUMN_NUM] = {2,4};

// Create the Keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

int first = 0;
int counter = 0;
String uID;

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
  mySerial.begin(9600);
  Serial.println("GPS start");
  delay(10);


  
  // pinMode(BL_PIN, OUTPUT);
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

  

 SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");

}//end of void setup


void loop() {

Serial.println("SCAN NOW"); 

  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print("SCAN NOW"); // display "scan now" on device screen
Serial.println("SCAN NOW"); 

  checkcard(); // Waiting for user to scan card


tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print(uID); // display caerd details on device screen


  getUserInput(); //Waits for user to enter amount and press the hash key, it also displays the amount

  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print("Sucessful"); // display "successful or failed" on device screen according to server message
Serial.println("Sucessful"); 


  while (mySerial.available() > 0)
  {
    if (gps.encode(mySerial.read()))
      displayInfo();
  }
/*
  Serial.print("LAT:  ");
  Serial.println(latitude, 6);  // float to x decimal places
  Serial.print("LONG: ");
  Serial.println(longitude, 6);

  //use latitude and longitude to send gps information to server
*/



  checkScreenTimeout();


    // Check if the keypad is pressed during screen timeout
  if (!screenActive && isKeypadPressed()) {
    //digitalWrite(BL_PIN, HIGH);  // Turn on the backlight
    Serial.println("Turn on the backlight");
    screenActive = true;
  }
}

bool isKeypadPressed() {
  char key = keypad.getKey();
  return key != NO_KEY;

/*
char key = keypad.getKey();

  if (key) {
    Serial.println("Key Pressed: " + String(key));
  }

  */

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
  Serial.println();
  
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


void displayMessage(String message, String inputField) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(message);
  
  // Display input field
  tft.setCursor(10, 50);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE); // You can choose a different color
  tft.println(inputField);

  lastActivityTime = millis(); // Update the last activity time
  screenActive = true; // Set the screen as active
  //digitalWrite(BL_PIN, HIGH);  // Turn on the backlight
  Serial.println("Turn on the backlight");
}



String getUserInput() {
  String amount = "";
  char key;
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key == '#') {
        break;
      }
      amount += key;
      displayMessage("Enter Amount:", "Amount: " + amount);
    }
  }
  return amount;
}

void checkScreenTimeout() {
  if (millis() - lastActivityTime > SCREEN_TIMEOUT) {
    //digitalWrite(BL_PIN, LOW);  // Turn off the backlight
    screenActive = false; // Set the screen as inactive
    Serial.println("Turn off the backlight");
  }
}
