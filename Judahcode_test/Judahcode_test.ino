/*#define DEBUG true
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




//******************* SIM Paramaters *******************/
/*
String SOS_NUM = "";
String APN_NAME = "internet.ng.airtel.com";

//******************* Necessary Variables *******************/
/*
String fromGSM = "";
String res = "";
char* response = " ";
String link1;
String link2;
String msg;

    //setupGPRS();
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


void StartTCP(void){
 //sendData("AT+CIPSTART=\"TCP\",\"secrets-api.appbrewery.com\",80\r\n",1000 , DEBUG);
 sendData("AT+HTTPGET=www.google.com", 3000 , DEBUG);

}

//void start_WAN_TCP_client(bool wireless_mode, char* APN_detail, char* ISP_username, char* ISP_password){ //GSM = 0 or WiFi = 1
void start_WAN_TCP_client(char* APN_detail, char* ISP_username, char* ISP_password){ //GSM = 0 or WiFi = 1
  

  
  //********* for GSM module ****************
  sendData("AT\r\n",1000 , DEBUG);
  delay(2000);
  
//  if(wireless_mode == true){
    //********* for GSM module ****************
  //  sendData("AT+CIPSHUT\r\n",1000, DEBUG);

   // delay(1000);
    sendData("AT+CIPMUX=0\r\n",1000 , DEBUG);
    
   delay(1000);
    
sendData("AT+CGATT=1", 1000, DEBUG);
    
    delay(1000);
sendData("AT+CGDCONT=1,\"IP\",\"" + String(APN_detail) + "\"\r\n", 3000, DEBUG);


 delay(1000);
 

sendData("AT+CGACT=1,1", 1000, DEBUG);

 
   
   /*
   // _delay_ms(1000);
    sendData("AT+CSTT=\"", 1000, DEBUG);

    //*** APN ***

    //USART_send_string("etisalat");
    sendData(APN_detail,1000 , DEBUG);
    //***********

    sendData("\",\"",1000 , DEBUG);


    //*** Username ***
    sendData(ISP_username,1000 , DEBUG);
    //***********


    sendData("\",\"", 0000, DEBUG);

    //*** Password ***

    sendData(ISP_password,1000 , DEBUG);
    //***********

    sendData("\"\r\n",1000 , DEBUG);
*//*
    //_delay_ms(1000);
    delay(1000);
    sendData("AT+CIICR",1000 , DEBUG);
    delay(1000);
    sendData("AT+CIFSR",1000 , DEBUG);
    delay(1000);
    sendData("AT+CSQ", 1000, DEBUG);

      StartTCP();
 }
  


bool waitForNetworkRegistration() {
    Serial.println("Checking network registration...");
    for (int i = 0; i < 10; i++) {
        String regStatus = sendData("AT+CREG?", 5000, DEBUG);
        if (regStatus.indexOf("+CREG: 1,1") != -1 || regStatus.indexOf("+CREG: 1,5") != -1) {
            Serial.println("Network registered.");
            return true;
        } else if (regStatus.indexOf("+CREG: 1,0") != -1) {
            Serial.println("Not registered, trying to register...");
            // Optionally, force a re-registration attempt here
            sendData("AT+CREG=1", 5000, DEBUG); // Enable network registration and make sure it's active
            sendData("AT+CFUN=1,1", 5000, DEBUG); // Restart the modem
        }
        delay(5000); // Increase delay to allow more time for registration attempt
    }
    Serial.println("Failed to register on network after multiple attempts.");
    return false;
}

void tcp_send_command(void){
  
  sendData("AT+CIPSEND\r\n",1000 , DEBUG);
}



void setup(void) {

Serial.begin(115200);// For ESP32
  Serial2.begin(115200); //For A9G
  
Serial.println("Started delay ");
delay (15000);
Serial.println("Stopped delay ");
Serial.println("Starting GPRS setup...");
//waitForNetworkRegistration();
start_WAN_TCP_client("web.gprs.mtnnigeria.net","","");

}
  



void loop() {
  //if (waitForNetworkRegistration()== true){
  //tcp_send_command();
delay (1500);
//sendData("/random",2000 , DEBUG); // Set content type as JSON
//sendData("\r\n",1000 , DEBUG); // close connection
 // }

  //else if (waitForNetworkRegistration()== false){
  // waitForNetworkRegistration();
   //start_WAN_TCP_client("web.gprs.mtnnigeria.net","","");
 
 // }
  
}*/

/*
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#define DEBUG true

// Your GPRS credentials, if any
const char apn[] = "internet.ng.airtel.com"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "";
const char gprsPass[] = "";


const char* topicinbound = "projx/inbound";
const char* topicoutbound = "projx/outbound";
const byte rxPin = 27;
const byte txPin = 26;


// set up a new serial object
SoftwareSerial mySerial (rxPin, txPin);

#include <Wire.h>



void setup() {
 // Set console baud rate
  delay(10);
 Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200);
 mySerial.begin(9600);
Serial.println("Wait...");
  delay(9000); 
  Serial.println("Done");
  delay(9000);
 

  // Set GSM module baud rate and UART pins
  //SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
}

//end of void setup
void loop() {
  
//int amountString = 500;
//String uID = "343fgf";
delay(9000);

  
  // Attempt the AT+COPS command
  if (!attemptCommand("AT+COPS=1,2,\"62120\"", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command.");
  }

 // Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }

    // Attempt the AT+SAPBR command
  String sapbrCommand = "AT+SAPBR=3,1,\"APN\",\"" + String(apn) + "\"";
  if (!attemptCommand(sapbrCommand, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }


  // Attempt the AT+SAPBR command
  String sapbrCommand1 = "AT+SAPBR=3,1,\"USER\",\"" + String(gprsUser) + "\"";
  if (!attemptCommand(sapbrCommand1, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command: " + sapbrCommand);
  }

 // Attempt the AT+SAPBR command
  String sapbrCommand2 = "AT+SAPBR=3,1,\"USER\",\"" + String(gprsPass) + "\"";
  if (!attemptCommand(sapbrCommand2, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command: " + sapbrCommand);
  }

  // Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=1,1", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }

// Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=2,1", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }

// Initialize HTTP
  if (!attemptCommand("AT+HTTPINIT", 1000, 3, "OK")) {
    Serial.println("Failed to initialize HTTP.");
  }
 
  if (!attemptCommand("AT+HTTPPARA=\"CID\",1", 1000, 3, "OK")) {
    Serial.println("Failed to set HTTP CID.");
 }

   if (!attemptCommand("AT+HTTPSSL=1", 1000, 3, "OK")) {
    Serial.println("Failed to set HTTP CID.");
  }

  
 if (!attemptCommand("AT+HTTPPARA=\"REDIR\",1", 1000, 3, "OK")) {
    Serial.println("Failed to REDIR URL.");
  }

  // Add User-Agent header
    if (!attemptCommand("AT+HTTPPARA=\"USERDATA\",\"User-Agent: ArduinoGSM/1.0\"", 1000, 3, "OK")) {
        Serial.println("Failed to set User-Agent.");
    }

        if (!attemptCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 1000, 3, "OK")) {
        Serial.println("Failed to set content type.");
    }

     // Adding an additional delay before setting the URL
    delay(2000);
  
  if (!attemptCommand("AT+HTTPPARA=\"URL\",\"http://ondigo.us-east-1.elasticbeanstalk.com/payment\"", 3000, 3, "OK")) {
    Serial.println("Failed to set HTTP URL.");
  }


 
  // Perform HTTP GET
  if (!attemptCommand("AT+HTTPACTION=1", 10000, 3, "+HTTPACTION: 0,200")) {
    Serial.println("HTTP GET request failed.");
  }

  // Read the HTTP response
  String response = sendData("AT+HTTPREAD", 10000, DEBUG);
  Serial.println("HTTP Response: " + response);

  // Terminate HTTP service
  if (!attemptCommand("AT+HTTPTERM", 1000, 3, "OK")) {
    Serial.println("Failed to terminate HTTP.");
  }

  // Deactivate PDP context
  if (!attemptCommand("AT+SAPBR=0,1", 1000, 3, "OK")) {
    Serial.println("Failed to deactivate PDP context.");
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
  mySerial.println(command);
  delay(500);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (mySerial.available())
    {
      char c = mySerial.read();
      temp += c;
    }
  }
  if (debug)
  {
    Serial.print(temp);
  }
  return temp;
}
*/
/*
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#define DEBUG true

// Your GPRS credentials, if any
const char apn[] = "internet.ng.airtel.com"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "";
const char gprsPass[] = "";


const char* topicinbound = "projx/inbound";
const char* topicoutbound = "projx/outbound";
const byte rxPin = 27;
const byte txPin = 26;


// set up a new serial object
SoftwareSerial mySerial (rxPin, txPin);

#include <Wire.h>



void setup() {
 // Set console baud rate
  delay(10);
 Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200);
 mySerial.begin(9600);
Serial.println("Wait...");
  delay(9000); 
  Serial.println("Done");
  delay(9000);
 

gsm_config_gprs();

  // Set GSM module baud rate and UART pins
  //SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
}

//end of void setup
void loop() {
  
//int amountString = 500;
//String uID = "343fgf";

gsm_connect_Server();

}

void gsm_connect_Server(){
// Initialize HTTP
  if (!attemptCommand("AT+HTTPINIT", 1000, 3, "OK")) {
    Serial.println("Failed to initialize HTTP.");
  }
 
  if (!attemptCommand("AT+HTTPPARA=\"CID\",1", 1000, 3, "OK")) {
    Serial.println("Failed to set HTTP CID.");
 }

   if (!attemptCommand("AT+HTTPSSL=1", 1000, 3, "OK")) {
    Serial.println("Failed to set HTTP CID.");
  }

  
 if (!attemptCommand("AT+HTTPPARA=\"REDIR\",1", 1000, 3, "OK")) {
    Serial.println("Failed to REDIR URL.");
  }

  // Add User-Agent header
    if (!attemptCommand("AT+HTTPPARA=\"USERDATA\",\"User-Agent: ArduinoGSM/1.0\"", 1000, 3, "OK")) {
        Serial.println("Failed to set User-Agent.");
    }

 if (!attemptCommand("AT+HTTPPARA=\"URL\",\"http://ondigo.us-east-1.elasticbeanstalk.com/payment\"", 3000, 3, "OK")) {
    Serial.println("Failed to set HTTP URL.");
  }

        if (!attemptCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"", 1000, 3, "OK")) {
        Serial.println("Failed to set content type.");
    }

       if (!attemptCommand("AT+HTTPPARA=\"=192,5000\"", 1000, 3, "OK")) {
        Serial.println("Failed to set content type.");
    }
 
  // Perform HTTP GET
  if (!attemptCommand("AT+HTTPACTION=1", 10000, 3, "+HTTPACTION: 0,200")) {
    Serial.println("HTTP GET request failed.");
  }

  // Read the HTTP response
  String response = sendData("AT+HTTPREAD", 10000, DEBUG);
  Serial.println("HTTP Response: " + response);

  // Terminate HTTP service
  if (!attemptCommand("AT+HTTPTERM", 1000, 3, "OK")) {
    Serial.println("Failed to terminate HTTP.");
  }

  // Deactivate PDP context
  if (!attemptCommand("AT+SAPBR=0,1", 1000, 3, "OK")) {
    Serial.println("Failed to deactivate PDP context.");
  }
}

void gsm_config_gprs(){
  delay(9000);
  
  // Attempt the AT+COPS command
  if (!attemptCommand("AT+COPS=1,2,\"62120\"", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command.");
  }

 // Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }

    // Attempt the AT+SAPBR command
  String sapbrCommand = "AT+SAPBR=3,1,\"APN\",\"" + String(apn) + "\"";
  if (!attemptCommand(sapbrCommand, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }


  // Attempt the AT+SAPBR command
  String sapbrCommand1 = "AT+SAPBR=3,1,\"USER\",\"" + String(gprsUser) + "\"";
  if (!attemptCommand(sapbrCommand1, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command: " + sapbrCommand);
  }

 // Attempt the AT+SAPBR command
  String sapbrCommand2 = "AT+SAPBR=3,1,\"USER\",\"" + String(gprsPass) + "\"";
  if (!attemptCommand(sapbrCommand2, 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the command: " + sapbrCommand);
  }

  // Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=1,1", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
  }

// Attempt the AT+SAPBR command
  if (!attemptCommand("AT+SAPBR=2,1", 2000, 3, "OK")) {
    Serial.println("Continuing after 3 failed attempts to execute the AT+SAPBR command.");
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
  mySerial.println(command);
  delay(500);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (mySerial.available())
    {
      char c = mySerial.read();
      temp += c;
    }
  }
  if (debug)
  {
    Serial.print(temp);
  }
  return temp;
}
*/
/*
#include <Arduino.h>
#include <SoftwareSerial.h>

 const byte rxPin = 27;
const byte txPin = 26;


// set up a new serial object
SoftwareSerial mySerial (rxPin, txPin);



void setup() {
  // initialize both serial ports:
  Serial.begin(115200);// For ESP32
   mySerial.begin(9600);

Serial.println("Wait...");

  delay(15000);
  Serial.println("Done");
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
*/




#include <Arduino.h>
#include <SPI.h>
#include <ArduinoJson.h>
#define DEBUG true

#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// or Software Serial on Uno, Nano

#include <SoftwareSerial.h>
SoftwareSerial mySerial(27, 26);  // RX, TX


#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

#define TINY_GSM_DEBUG SerialMon

#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]      = "internet.gprs.airtel.com";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

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
StreamDebugger debugger(mySerial, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(mySerial);
#endif

TinyGsmClient client(modem);
HttpClient   http(client, serverName, port);
//const byte rxPin = 27;
//const byte txPin = 26;
  


// set up a new serial object
//SoftwareSerial mySerial (rxPin, txPin);

#include <Wire.h>



void setup() {
 // Set console baud rate
  delay(10);


// Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!

  SerialMon.println("Wait...");

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
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif
}




//end of void setup
void loop() {



// Serializing in JSON Format
  DynamicJsonDocument docc(1024);
  docc["UID"] = "frankyjayde";
  docc["amount"] = "500";
  docc["machineId"] = "dres500";
  String msg;
  serializeJson(docc, msg);
  SerialMon.println(msg);


  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) { SerialMon.println("GPRS connected"); }



  SerialMon.print(F("Performing HTTPS GET request... "));
 
  String contentType = "application/json"; // "application/x-www-form-urlencoded"; 
  const char resource[] = "/payment";
  http.connectionKeepAlive();  // Currently, this is needed for HTTPS

   //http.sendHeader("Transfer-Encoding", "Chunked"); 
   int err = http.post(resource, contentType, msg);
   
        
  if (err != 0) {
    SerialMon.println(F("failed to connect"));
    SerialMon.println(err);
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.print(F("Response status code: "));
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  SerialMon.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName  = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    SerialMon.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));


  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  // Do nothing forevermore
  while (true) { delay(1000); }
} 
