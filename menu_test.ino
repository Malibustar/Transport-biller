#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int selected = 0;
int entered = -1;
int selectedTfare = 0;
int enteredTfare = -1;

void displaymenu(void) {

  int down = digitalRead(17);
  int up = digitalRead(2);
  int enter = digitalRead(16);
  int back = digitalRead(5);

  if (up == LOW && down == LOW) {
  };
  if (up == LOW) {
    selected = (selected + 1)  % 6; // Adjust 7 to the total number of menu items;
    selectedTfare = (selectedTfare + 1)  % 2; // Adjust 7 to the total number of menu items;
    delay(200);
  };
  if (down == LOW) {
    selected = (selected - 1 + 6) % 6; // Adjust 7 to the total number of menu items;
    selectedTfare = (selectedTfare - 1 + 2) % 2; // Adjust 7 to the total number of menu items;
    delay(200);
  };
  if (enter == LOW) {
    entered = selected;
    enteredTfare = selectedTfare;
  };
  if (back == LOW) {
    entered = -1;
    enteredTfare = -1;
  };

  
  const char *options[7] = {
    " COLLECT FARE ",
    " WALLET BALANCE ",
    " WITHDRAW ",
    " MESSAGES ",
    " TEST CONNECTION ",
    " ADMIN ",
    " SETTINGS "
  };

    const char *collectFareOptions[2] = {
    " FIXED PAY ",
    " VARIABLE PAY ",
  };

  //int collectFareSelected = 0;

  if (entered == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("MENU"));
    display.println("");

    for (int i = 0; i < 7; i++) {
      if (i == selected) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(options[i]);
      } else if (i != selected) {
        display.setTextColor(SSD1306_WHITE);
        display.println(options[i]);
      }
    }
  } 
  
  
  else if (entered == 0) { 
   // selected =  0;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Collect Tfare>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

  for (int i = 0; i < 2; i++) {
      if (i == selectedTfare ) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(collectFareOptions[i]);
      } 
      else if (i != selectedTfare) {
        display.setTextColor(SSD1306_WHITE);
        display.println(collectFareOptions[i]);
      }
    
    }
  }
  //selected =  0;

  else if (enteredTfare == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Current wallet balance>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println(" NGN10,000 ");
  }

  else if (enteredTfare == 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Enter amount to withdraw>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println(" NGN5000 ");

  }
 

   else if (entered == 3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Signal Strength>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("100%");

  } 
  
  else if (entered == 4) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Admin Settings>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.println(" 1. Recent Transactions");
    display.println(" 2. Withdrawal Log ");
    display.println(" 3. Pending transactions ");
    display.println(" 4. Update software ");

  } 
  
  else if (entered == 5) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("Messages>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.println(" Kindly update your device");
  } 
  
  else if (entered == 6) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Ondigo menu>"));
    display.println("System settings>");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.println("Date and time");
  }


  display.display();
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  // put your main code here, to run repeatedly:
  displaymenu();
}