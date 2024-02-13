
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <MFRC522.h>


// Select your modem:
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800L

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// set GSM PIN, if any
#define GSM_PIN ""


// Your GPRS credentials, if any
const char apn[] = "inernet.ng.airtel.com"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "web";
const char gprsPass[] = "web";

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// MQTT details
const char* broker = "broker.hivemq.com";                    // Public IP address or domain name
const char* mqttUsername = "REPLACE_WITH_YOUR_MQTT_USER";  // MQTT username
const char* mqttPassword = "REPLACE_WITH_YOUR_MQTT_PASS";  // MQTT password

const char* topicinbound = "projx/inbound";
const char* topicoutbound = "projx/outbound";

//Pin for ESP32
  #define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18

#define SS_PIN 5  
#define RST_PIN 0

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif


#include <PubSubClient.h>

TinyGsmClient client(modem);
PubSubClient mqtt(client);

// TTGO T-Call pins
// #define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
// #define I2C_SDA              21
// #define I2C_SCL              22

uint32_t lastReconnectAttempt = 0;

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

long lastMsg = 0;
int count = 0;
float buttonTimer = 0;
bool buttonPressed = false;
String pressedCharacter = "";
bool stringAdded = false;
String amountString = "";

#define BL_PIN 33  // Backlight pin, adjust this according to your wiring

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


void mqttCallback(char* topic, byte* message, unsigned int len) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < len; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic projx/inbound, you check if the message is either "true" or "false". 
  // Changes the output state according to the message
  if (String(topic) == "projx/inbound") {
    Serial.print("recieved message ");
    if(messageTemp == "1"){
      Serial.println("successful");
    }
    else if(messageTemp == "2"){
      Serial.println("unsuccessful");
    }
     else if(messageTemp == "3"){
      Serial.println("User Invalid");
    }
  }


}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker without username and password
  boolean status = mqtt.connect("GsmClientN");

  // Or, if you want to authenticate MQTT:
 //  boolean status = mqtt.connect("GsmClientN", mqttUsername, mqttPassword);

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  SerialMon.println(" success");
  mqtt.subscribe(topicoutbound);

  return mqtt.connected();
}


void setup() {
  Serial.begin(115200);
 // Set console baud rate
  delay(10);
  
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
//  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
//  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

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
 tft.setRotation(1); 


  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("Booting..."); // display "scan now" on device screen
  Serial.println("Booting..."); 


SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  // modem.restart();
  modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

   SerialMon.print("Connecting to APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    //ESP.restart();
  }
  else {
    SerialMon.println(" OK");
  }
  
  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);



  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");

tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(15, 0); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Signal strength " + String(modem.getSignalQuality()) + "%" ); // display "menu" on device screen
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
Serial.println("mqtt1");
mqtt.publish(topicoutbound, deviceinfo.c_str());
Serial.println("mqtt2");
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing payment.."); // display processing on device screen
mqtt.loop();
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
