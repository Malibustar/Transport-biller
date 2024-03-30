void readKeyPad();

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
