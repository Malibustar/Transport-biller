
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

#define OUTPUT_1 21
#define OUTPUT_2 22

// Your GPRS credentials, if any
const char apn[] = "web.gprs.mtnnigeria.net"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "web";
const char gprsPass[] = "web";

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// MQTT details
const char* broker = "broker.hivemq.com";                    // Public IP address or domain name
const char* mqttUsername = "REPLACE_WITH_YOUR_MQTT_USER";  // MQTT username
const char* mqttPassword = "REPLACE_WITH_YOUR_MQTT_PASS";  // MQTT password

const char* topicOutput1 = "esp/output1";
const char* topicOutput2 = "esp/output2";
const char* topicTemperature = "esp/temperature";
const char* topicHumidity = "esp/humidity";

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

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
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22


uint32_t lastReconnectAttempt = 0;

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

char temperature = 0;
char humidity = 0;
long lastMsg = 0;


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

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp/output1, you check if the message is either "true" or "false". 
  // Changes the output state according to the message
  if (String(topic) == "esp/output1") {
    Serial.print("Changing output to ");
    if(messageTemp == "true"){
      Serial.println("true");
      digitalWrite(OUTPUT_1, HIGH);
    }
    else if(messageTemp == "false"){
      Serial.println("false");
      digitalWrite(OUTPUT_1, LOW);
    }
  }
  else if (String(topic) == "esp/output2") {
    Serial.print("Changing output to ");
    if(messageTemp == "true"){
      Serial.println("true");
      digitalWrite(OUTPUT_2, HIGH);
    }
    else if(messageTemp == "false"){
      Serial.println("false");
      digitalWrite(OUTPUT_2, LOW);
    }
  }
}

boolean mqttConnect() {
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker without username and password
  //boolean status = mqtt.connect("GsmClientN");

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("GsmClientN", mqttUsername, mqttPassword);

  if (status == false) {
    SerialMon.println(" fail");
    ESP.restart();
    return false;
  }
  SerialMon.println(" success");
  mqtt.subscribe(topicOutput1);
  mqtt.subscribe(topicOutput2);

  return mqtt.connected();
}


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);
  
  
  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  
  pinMode(OUTPUT_1, OUTPUT);
  pinMode(OUTPUT_2, OUTPUT);
  
  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

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
    ESP.restart();
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
}

void loop() {
  if (!mqtt.connected()) {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;
    
    
    char tempString [8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    mqtt.publish(topicTemperature, tempString);

    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    mqtt.publish(topicHumidity, humString);
  }

  mqtt.loop();
}