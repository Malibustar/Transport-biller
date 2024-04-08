
// Function to get radio signal strength and battery level
void getSignalAndBattery(int &bat, int &sig) {
  serial1.println("AT+CSQ"); // Send command to get radio signal strength
  delay(10); // Wait for response
  if (serial1.available() > 0) {
    sig = serial1.parseInt(); // Read the signal strength value
  }

  serial1.println("AT+CBC"); // Send command to get battery level
  delay(10); // Wait for response
  if (serial1.available() > 0) {
    bat = serial1.parseInt(); // Read the battery level value
  }
}





