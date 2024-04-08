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

#define BL_PIN 26
#define LED 33
#define Buzzer 14

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


 int febucheck;
int day=00;int month=00;int year=00;int hour=00;int mint=00;int seconds;

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


//******************* MQTT Parameters *******************//

String MQTT_BROKER = "broker.hivemq.com";
String MQTT_PORT = "1883";
String MQTT_USERNAME = "";
String MQTT_PASSWORD = "";


//******************* SIM Paramaters *******************//

String SOS_NUM = "";
String APN_NAME = "internet.ng.airtel.com";


//******************* Data Sending Interval *******************//
uint16_t Send_Data_After = 60; // 60 sec waiting

//******************* Necessary Variables *******************//

String fromGSM = "";
String res = "";
char* response = " ";
String location_data;
String lats;
String longi;
String link1;
String link2;
String msg;

// define two tasks for MQTT_Task & SOSButton_Task
void MQTT_Task( void *pvParameters );
void SOSButton_Task( void *pvParameters );
void setTimeFromGSM();
void nettime();

void MQTT_ReConnect()
{

  String new_msg = "";
  new_msg = sendData("AT+MQTTDISCONN", 2000, DEBUG);
  while ( new_msg.indexOf("OK") == -1 ) {
    new_msg = sendData("AT+MQTTDISCONN", 2000, DEBUG);
    Serial.println("Trying");
  }


  new_msg = "";
  new_msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, DEBUG);
  while ( new_msg.indexOf("OK") == -1 ) {
    new_msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }

}

String sendData(String command, const int timeout, boolean debug)
{
  String temp = "";
  Serial2.println(command);
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

void MQTT_CONNECT()
{
msg = "";
  msg = sendData("AT", 1000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+MQTTDISCONN", 1000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+MQTTDISCONN", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+CGATT=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGATT=1", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+CGDCONT=1,\"IP\",\"" + APN_NAME + "\"", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGDCONT=1,\"IP\",\"" + APN_NAME + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }


  msg = "";
  msg = sendData("AT+CGACT=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGACT=1", 1000, DEBUG);
    Serial.println("Trying");
  }


  msg = "";
  msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }

}

// the setup function runs once when you press reset or power the board
void setup(void)
{

  Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200); // For ESP32
  Serial2.begin(115200); //For A9G
  
  Serial.println("ILI9341 Test!"); 

  pinMode(BL_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  delay(100);
 digitalWrite(BL_PIN, HIGH);
 digitalWrite(LED, HIGH); 
 
  
    // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  tft.setRotation(1);
  tft.invertDisplay(false);

 tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame

  Serial.println("Before Reset");
  delay(15000);// Waiting For 15 Sec for Initialisation

  msg = "";
  msg = sendData("AT+RST=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+RST = 1", 2000, DEBUG);
    Serial.println("Trying");
  }

  Serial.println("Before Delay");
  delay(15000);// Waiting For 15 Sec for Initialisation

  Serial.println("After Delay");

  

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    MQTT_Task
    ,  "MQTT_Task"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  0);

  xTaskCreatePinnedToCore(
    SOSButton_Task
    ,  "SOSButton_Task"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL
    ,  0);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void MQTT_Task(void *pvParameters)  // This is a task.
{
  (void) pvParameters;


  for (;;) // A Task shall never return or exit.
  {
    
    uID = "";
amountString = "";



//enterAmount(); // calls enter amount function for user to input amount

if(gettime==0){
  // Initialize time
  setTimeFromGSM(); // Get time from GSM module
   }

if (gettime==1){
nettime();

}

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
tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(48, 16, internet, 15, 16, ST77XX_WHITE);
tft.drawBitmap(275, 13, battery_83, 24, 16, ST77XX_WHITE);


    /*
      String coordinates = "0123232" ;
      Serial.print("coordinates - "); Serial.print(coordinates); Serial.println("-");

      link1 = ("AT+MQTTPUB=\"franky\",\"" + coordinates + "\",0,0,0") ;
      //Serial2.println("AT+MQTTPUB=\"A9G\",\"12.34567890123456789.0123456\",0,0,0");
      Serial.print("link lat -"); Serial.println(link1);

      //Serial.print("For Serial Monitor-"); Serial.println(link1);

      // Serial2.println(link1);
      sendData(link1, 1000, DEBUG);

      delay(2000);

      Serial.println("Location DataSend");

    

    msg = "";
    msg = sendData("AT+CBC?", 2000, DEBUG);
    while ( msg.indexOf("OK") == -1 ) {
      msg = sendData("AT+CBC?", 1000, DEBUG);
      Serial.println("Trying");
    }
    Serial.print("Recevied Data Before - "); Serial.println(msg); // printin the String in lower character form
    int count = 0;
    while (msg[count] != ',')
    {
      count++;
      Serial.print(msg[count]);

    }

    msg = msg.substring(count + 2);

    count = 0;
    while (msg[count] != '\n')
    {
      count++;
      Serial.print(msg[count]);

    }

    msg = msg.substring(0, count - 1);

    Serial.print("Recevied Data - "); Serial.println(msg); // printin the String in lower character form
    Serial.println("\n");

    link2 = ("AT+MQTTPUB=\"franky\",\"" + msg + "\",0,0,0") ;
    Serial.print("battery link -"); Serial.println(link2);
    Serial.print("For Serial Monitor-"); Serial.println(link2);

    // Serial2.println(link1);
    msg = "";
    msg =  (sendData(link2, 1000, DEBUG));
    char* msg_char = &msg[0];
    Serial.print("LAT MSG - "); Serial.println(msg_char);
    if ( !(strstr(msg_char, "OK" ))) {
     // MQTT_ReConnect();
    }
    delay(2000);

    Serial.println("Battery DataSend");

    response = "";
    res = "";


    Serial.println("Delay");

   vTaskDelay((Send_Data_After * 1000));
   
  }*/
}
}

void SOSButton_Task(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /*
    AnalogReadSerial
    Reads an analog input on pin A3, prints the result to the serial monitor.
    Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
    Attach the center pin of a potentiometer to pin A3, and the outside pins to +5V and ground.

    This example code is in the public domain.
  */

  for (;;)
  {
  vTaskDelay(10);  // one tick delay (15ms) in between reads for stability
  }

readKeyPad(); //read the keypad
  
  if (buttonPressed == true) //if a button was pressed...
  {
  enterAmount();
  }

//if (checkcard()==true && checkNetwork()==true) {
if (checkcard()==true) {
String paymentinfo = amountString + "," + uID;
tft.fillScreen(ST77XX_BLACK);
tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(3);
tft.print("Processing payment.."); // display processing on device screen

      Serial.print("payment info - "); Serial.print(paymentinfo); Serial.println("-");

      link1 = ("AT+MQTTPUB=\"ondigopayment\",\"" + paymentinfo + "\",0,0,0") ;
      
      Serial.print("Payment details -"); Serial.println(link1);

      //Serial.print("For Serial Monitor-"); Serial.println(link1);

      // Serial2.println(link1);
      sendData(link1, 1000, DEBUG);

      delay(2000);

      Serial.println("payment Made");

  tft.fillScreen(ST77XX_BLACK);
tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(3);
tft.print("Payment Successful.."); // display processing on device screen


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
        pressedCharacter = "1";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101101 - P1 pin went low after pressing the button -> 2 was pressed
      case 237: //Button 2
        Serial.print("4");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "4";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101011 - P2 pin went low after pressing the button -> 3 was pressed
      case 235: //Button 3
        Serial.print("7");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "7";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11100111 - P3 pin went low after pressing the button -> A was pressed
      case 231: //Button A
        Serial.println("*");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "*";
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
        Serial.print("2");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "2";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 221: //Button 5
        Serial.print("5");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "5";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 219: //Button 6
        Serial.print("8");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "8";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 215: //Button B
        Serial.println("0");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "0";
        buttonPressed = true;
        stringAdded = true;
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
        pressedCharacter = "3";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 189: //Button 8
        Serial.print("6");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "6";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 187: //Button 9
        Serial.print("9");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "9";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 183: //Button C
        Serial.println("#");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "#";
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
        Serial.print("A");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "A";
        buttonPressed = true;
        break;

      case 125: //Button 0
        Serial.print("B");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "B";
        buttonPressed = true;
        break;

      case 123: //Button #
        Serial.print("C");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "C";
        buttonPressed = true;
        break;

      case 119: //Button D
        Serial.println("D");
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        pressedCharacter = "D";
        buttonPressed = true;
        break;
    }
    buttonTimer = millis();
  }
}

void setTimeFromGSM() {
  while (Serial2.available()) {
    Serial2.println("AT+CCLK?");
    delay(1000); // Wait for response
    String response = Serial2.readStringUntil('\n');
    Serial.println("Received response: " + response);

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


      // Exit the loop after successfully setting the time
      gettime = 1;
      Serial.println("Time set successfully.");
      break;
    } else {
      Serial.println("Invalid response format.");
    }
  }
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


/*
void getBatteryState(){
tft.drawBitmap(275, 124, battery_17, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 104, battery_33, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 82, battery_50, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 60, battery_67, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 38, battery_83, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 17, battery_full, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 147, charger_connected, 24, 16, ST77XX_WHITE);
tft.drawBitmap(19, 86, network_1_bar, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 61, network_2_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 38, network_3_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 110, network_not_connected, 15, 16, ST77XX_WHITE);
tft.drawBitmap(48, 15, internet, 15, 16, ST77XX_WHITE);
tft.drawBitmap(79, 14,location_icon, 13, 16, ST77XX_WHITE);

while (Serial2.available()) {

    Serial.write("AT+CBC?");
    delay(100);
     String response = Serial2.readStringUntil('\n');
    Serial.println("Response: " + response);

    // Parse the response
    int chargerState, batteryPercentage;
    if (sscanf(response.c_str(), "+CBC: %d,%d", &chargerState, &batteryPercentage) == 2) {
      // Print charging state
      Serial.print("Charging state: ");
      Serial.println(chargerState == 0 ? "Connected" : "Not connected");

      // Map battery percentage to 6 stages
      int batteryStage = map(batteryPercentage, 0, 100, 0, 5);
      Serial.print("Battery stage: ");
      switch (batteryStage) {
        case 0:
          Serial.println("0-16%");
          break;
        case 1:
          Serial.println("17-33%");
          break;
        case 2:
          Serial.println("34-49%");
          break;
        case 3:
          Serial.println("50-66%");
          break;
        case 4:
          Serial.println("67-83%");
          break;
        case 5:
          Serial.println("84-100%");
          break;
        default:
          Serial.println("Unknown");
      }
    } else {
      Serial.println("Failed to parse response.");
    }
  } else {
    Serial.println("No response received.");
  }
}
*/


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

 if (pressedCharacter == "C" ) { // Delete on "C" press if string has characters
     tft.setTextColor(ST77XX_BLACK);
      tft.setCursor(115, 120);
     tft.print(amountString);
     amountString = "";
     
 
      }
      
    // Check if the "C" key is pressed to exit the function
    if (buttonPressed && pressedCharacter == "#") {
      buttonPressed = false; // Reset buttonPressed to avoid immediate triggering
      Serial.println("Amount Entered");
      
      checkcard();


  //tft.fillScreen(ST77XX_BLACK);
  //tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
      return; // Exit the function
    }

 if (pressedCharacter == "B" ) { // Delete on "C" press if string has characters
        amountString = "";
      tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame
  return; // Exit the function
      }
 
  }
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

;

  counter = counter + 1;
  Serial.print(counter);

  //}

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