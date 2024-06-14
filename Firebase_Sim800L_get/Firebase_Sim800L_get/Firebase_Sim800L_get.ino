#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient
#include <ArduinoJson.h>

#include <SoftwareSerial.h>
SoftwareSerial sim800(27, 26);
 

 
const char FIREBASE_HOST[]  = "ondigo.us-east-1.elasticbeanstalk.com/payment";
const String FIREBASE_AUTH  = "ht26AiTCegllGDThV9wf8lqdUGaSlakNiRIJlig8";
const String FIREBASE_PATH  = "/";
const int SSL_PORT          = 80;
 
char apn[]  = "internet.gprs.airtel.com";  // type your APN here
char user[] = "";
char pass[] = "";
 
 
TinyGsm modem(sim800);
 
TinyGsmClient gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);
 
unsigned long previousMillis = 0;

 
void setup()
{
  Serial.begin(115200);
  sim800.begin(9600);
  Serial.println("\nInitializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  http_client.setHttpResponseTimeout(10 * 300); //^0 secs timeout
}
 
void loop()
{
 
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" OK");
 
  http_client.connect(FIREBASE_HOST, SSL_PORT);
 
  while (true) {
    if (!http_client.connected())
    {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else
      GetFirebase(&http_client);
     
  }
 
}
 
void GetFirebase( HttpClient* http)
{
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS

 /* String url;
  if (path[0] != '/')
  {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;

   Serial.println(url);*/
   /*/.json?auth=ht26AiTCegllGDThV9wf8lqdUGaSlakNiRIJlig8

  http->post(url);*/
// Serializing in JSON Format
  DynamicJsonDocument docc(1024);
  docc["UID"] = "frankyjayde";
  docc["amount"] = "500";
  docc["machineId"] = "dres500";
  String msg;
  serializeJson(docc, msg);
  Serial.println(msg);



  http->beginRequest();
  http->post("/payment"); // Post to the root path or an appropriate endpoint
  http->sendHeader("Content-Type", "application/json");
  http->sendHeader("Content-Length", msg.length());
  http->beginBody();
  http->print(msg);
  http->endRequest();

  
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
 
  Serial.print("Response: ");
  Serial.println(response);

  if (!http->connected())
  {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
 
}
 
