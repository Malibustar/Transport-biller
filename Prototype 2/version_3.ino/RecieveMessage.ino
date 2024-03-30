// Function to return device menu/setting from server
void recieveMessage(String& Device_ID, String& TFARE, String& WALLETBAL, String& ADMIN) {
  String sub_message = "";

  // Subscribe to MQTT topic
  serial1.println("AT+MQTTSUB=\"/public/TEST/makerfabs-B\",1,0");

  // Wait for message to arrive
  delay(1000); // Adjust delay as needed based on MQTT message arrival time

  while (serial1.available() > 0) {
    char c = serial1.read();
    sub_message += c;
  }

  // Parse received JSON message
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, sub_message);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract string values from JSON and assign them to provided references
  Device_ID = doc["DEVICEID"].as<String>();
  TFARE = doc["COLLECT_TFARE"].as<String>();
  WALLETBAL = doc["WALLET_BALANCE"].as<String>();
  ADMIN = doc["ADMIN_SET"].as<String>();

  // Print extracted values for debugging
  Serial.print("Device ID: ");
  Serial.println(Device_ID);
  Serial.print("Collect TFARE: ");
  Serial.println(TFARE);
  Serial.print("Wallet Balance: ");
  Serial.println(WALLETBAL);
  Serial.print("Admin Set: ");
  Serial.println(ADMIN);
}
