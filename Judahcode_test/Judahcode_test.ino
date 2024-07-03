

#include <otadrive_esp.h>
#include <Arduino.h>
#include <SPI.h>
#include <ArduinoJson.h>
#define DEBUG true

#include <SoftwareSerial.h>
SoftwareSerial mySerial(27, 26);  // RX, TX

// install TinyGSM library (v0.11.7 or newer)
#define TINY_GSM_MODEM_SIM800

#if !defined(TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 650
#endif

#define TINY_GSM_DEBUG Serial

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

// Server details
const int  port       = 80;
const char* serverName = "dev.ondigo-ng.com";


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


#define APIKEY "6a694e12-b2f6-4b52-a93f-b30421e5d8f7" // OTAdrive APIkey for this product
#define FW_VER "v@8.21.8"                             // this app version

//TinyGsm modem(Serial);
// MuxChannel 0 for MQTT
TinyGsmClient gsm_mqtt_client(modem, 0);
// MuxChannel 1 for OTA
TinyGsmClient gsm_otadrive_client(modem, 1);
HttpClient   http(gsm_mqtt_client, serverName, port);
String gprs_ip;

void printInfo()
{
  Serial.printf("Example08: OTA through GSM. %s, Serial:%s, IP:%s\r\n\r\n", FW_VER, OTADRIVE.getChipId().c_str(), gprs_ip.c_str());
}

void update_prgs(size_t i, size_t total)
{
  Serial.printf("upgrade %d/%d   %d%%\r\n\r\n", i, total, ((i * 100) / total));
}

void setup()
{

  
// Set console baud rate
  Serial.begin(115200);
  delay(10);
printInfo();
  Serial.println("Wait...");

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
   mySerial.begin(9600);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);

#if TINY_GSM_USE_GPRS
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }
#endif

  SPIFFS.begin(true);
  OTADRIVE.setInfo(APIKEY, FW_VER);
  OTADRIVE.onUpdateFirmwareProgress(update_prgs);

  // do something about power-on GSM
}

void loop()
{

  
// Serializing in JSON Format
  DynamicJsonDocument docc(1024);
  docc["UID"] = "132360AD";
  docc["amount"] = "500";
  docc["machineId"] = "ondigo00000000";
  String msg;
  serializeJson(docc, msg);
  Serial.println(msg);


  // GPRS connection parameters are usually set after network registration
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" success");

  if (modem.isGprsConnected()) { Serial.println("GPRS connected"); }


    
  if (OTADRIVE.timeTick(30))
  {
    Serial.println("Lets updaete the firmware\r\n");
    if (modem.isGprsConnected() || modem.isGprsConnected())
    {
      // auto a = OTADRIVE.updateFirmwareInfo(gsm_otadrive_client);
      // Serial.printf("info: %d, %d, %s\n", a.available, a.size, a.version.c_str());
      // auto c = OTADRIVE.getConfigs(gsm_otadrive_client);
      // Serial.printf("config %s\n", c.c_str());
      // OTADRIVE.sendAlive(gsm_otadrive_client);
      Serial.println("isGprsConnected\r\n");
      OTADRIVE.updateFirmware(gsm_otadrive_client);
    }
    else
    {
      Serial.println("Modem is not ready\r\n");
    }
  }
  else
  {
    Serial.println("Waiting\r\n");
  }

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


  //String json;
  //DynamicJsonDocument doc(1024);
  //body = msg.c_str();
  DeserializationError error = deserializeJson(docc, body);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

  String trans_code = docc["code"];
  String machineID = docc["machineId"];
  String status_code = docc["status"];

  Serial.println(trans_code);
  Serial.println(machineID);
  Serial.println(status_code);

  if (trans_code == "2" && status_code == "success"){
    Serial.println("transaction successful");
    }
   else if (trans_code == "3" && status_code == "Failed"){
    Serial.println("Insufficient fund");
    }
    else {
      Serial.println("unrecognized error");
      }
  Serial.print(F("Body length is: "));
  Serial.println(body.length());

  // Shutdown

  http.stop();
  Serial.println(F("Server disconnected"));


  modem.gprsDisconnect();
  Serial.println(F("GPRS disconnected"));

  // Do nothing forevermore
  while (true) { delay(1000); }

  
}
