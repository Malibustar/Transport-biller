void sendMoney() {

  serial1.println("AT+CGACT=1,1");      // Active the PDP
  delay (100);
  // Serializing in JSON Format
  DynamicJsonDocument doc(1024);

  // Assigning values to JSON object
  doc["MACHINEID"] = machineserial;
  doc["CARDID"] = uID;
  doc["AMOUNT"] = amountString;

  // Serialize JSON object to string
  String msg;
  serializeJson(doc, msg);

  // Publish JSON message to MQTT topic
  String mqttTopic = "/public/TEST/makerfabs-W";
  String mqttMessage = "AT+MQTTPUB=\"" + mqttTopic + "\",\"" + msg + "\",0,0,0";
  serial1.println(mqttMessage);

delay (500);

  // Subscribe to MQTT topic
  serial1.println("AT+MQTTSUB=\"/public/TEST/makerfabs-B\",1,0");

  // Wait for message to arrive
  delay(1000); // Adjust delay as needed based on MQTT message arrival time

  while (serial1.available() > 0) {
    char c = serial1.read();
    sub_message += c;
  }

  // Parse received JSON message
  //DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, sub_message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract string values from JSON and assign them to provided references
  SERDEVICEID = doc["DEVICEID"].as<String>();
  RESPONSE = doc["SERVERRESPONSE"].as<String>();
  

  // Print extracted values for debugging
  Serial.print("Device ID: ");
  Serial.println(SERDEVICEID);
  Serial.print("Collect TFARE: ");
  Serial.println(RESPONSE);

if (RESPONSE == "1"){

tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing successful.."); // display processing on device screen
 delay(100);
tft.fillScreen(ST77XX_BLACK);
Serial.println("Processing successful..");
//Serial.print(deviceinfo);

}
delay (10);

if (RESPONSE == "2"){

tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Processing Unsuccessful.."); // display processing on device screen
 delay(100);
tft.fillScreen(ST77XX_BLACK);
Serial.println("Processing Unsuccessful..");
//Serial.print(deviceinfo);
  
}

if (RESPONSE == "3"){

tft.fillScreen(ST77XX_BLACK);
tft.setCursor(0, 57); 
tft.setTextColor(ST77XX_WHITE);
tft.setTextSize(1);
tft.print("Invalid User.."); // display processing on device screen
 delay(100);
tft.fillScreen(ST77XX_BLACK);
Serial.println("Invalid User..");
//Serial.print(deviceinfo);
  
}

}
