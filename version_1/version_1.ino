#include <Wire.h>
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <TinyGPSPlus.h>
#include <GSM.h>

// These pins will also work for the 1.8" TFT shield
 
//ESP32-WROOM
#define TFT_DC 12 //A0
#define TFT_CS 13 //CS
#define TFT_MOSI 14 //SDA
#define TFT_CLK 27 //SCK
#define TFT_RST 0 
#define TFT_MISO 0

#define SS_PIN 2
#define RST_PIN 0

#define BUZZER_PIN 14

#define RXD2 17
#define TXD2 16

#define GSM_PIN_TX 27
#define GSM_PIN_RX 26

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_SERVER "mqtt_server_ip"
#define MQTT_PORT 1883
#define MQTT_USER "your_mqtt_username"
#define MQTT_PASSWORD "your_mqtt_password"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad keypad = Keypad(makeKeymap("123A456B789C*0#D"), 4, 4);
TinyGPSPlus gps;
GSM gsmAccess;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  tft.begin();

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  connectGSM();
}

void loop() {
  displayMessage("Scan NFC");

  if (scanCard()) {
    String amount = getUserInput();
    sendTransactionData(amount);
    beepBuzzer();
    displayMenu();
  }
}

void displayMessage(String message) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(message);
}

bool scanCard() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    return true;
  }
  return false;
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
      displayMessage("Amount: " + amount);
    }
  }
  return amount;
}

void sendTransactionData(String amount) {
  // Implement GSM module communication and MQTT data sending here
}

void beepBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void displayMenu() {
  while (true) {
    char key = keypad.getKey();
    if (key) {
      switch (key) {
        case '1':
          loop(); // Scan again
          break;
        case '2':
          // Display transaction history
          break;
        case '3':
          // Exit
          tft.fillScreen(ILI9341_BLACK);
          tft.setCursor(10, 10);
          tft.setTextSize(2);
          tft.setTextColor(ILI9341_WHITE);
          tft.println("Goodbye!");
          delay(2000);
          ESP.restart();
          break;
      }
    }
  }
}

void connectGSM() {
  // Implement GSM module connection here
}

void connectMQTT() {
  // Implement MQTT connection here
}


void GPSinfo() 
{

  /*we have first called the updateSerial() function, and comment it out, you can uncomment this if you are running the 
  device for the first time or you are debugging the GPS module. Next, we check if serial2 is available or not. 
  If serial2 is available, this function in returns gives us the lat and long data.*/
  
  //updateSerial();
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      
  Serial.print(F("Location: "));
  if (gps.location.isValid()){
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
}



/*void updateSerial(){
  delay(500);
 // This function is a loopback between the UART1 and UART2 so that we can monitor 
 //the incoming data out of the serial monitor window.
  while (Serial.available())  {
    Serial2.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (Serial2.available())  {
    Serial.write(Serial2.read());//Forward what Software Serial received to Serial Port
  }
}*/
