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
int currentMenuLevel = 1; // Variable to track the current menu level

int screen_page = -1;
int menu_option[5] = {-1,-1,-1,-1,-1};  //menu options count for 5 pages
bool enter_pressed = false;
bool page_shown_flag = true;

const char *options[7] = {
  " COLLECT FARE ",
  " WALLET BALANCE ",
  " WITHDRAW ",
  " MESSAGES ",
  " TEST CONNECTION ",
  " ADMIN ",
  " SETTINGS "
};

void displaymenu(void) {

  int down = digitalRead(17);
  int up = digitalRead(2);
  int enter = digitalRead(16);
  int back = digitalRead(5);

  if (up == LOW && down == LOW) {
  };
  if (up == LOW) {

    enter_pressed = true;

    switch(screen_page){
      case 0:
        if(menu_option[screen_page] < 5){ //5 is the maximum of options on the page
          menu_option[screen_page]++;
        }
        break;
      case 1:
        if(menu_option[screen_page] < 3){ //3 is the maximum of options on the page
          menu_option[screen_page]++;
        }
        break;
      case 2:
        if(menu_option[screen_page] < 6){ //6 is the maximum of options on the page
          menu_option[screen_page]++;
        }
        break;
      case 3:
        if(menu_option[screen_page] < 2){ //2 is the maximum of options on the page
          menu_option[screen_page]++;
        }
        break;
      case 4:
        if(menu_option[screen_page] < 4){ //4 is the maximum of options on the page
          menu_option[screen_page]++;
        }
    }

    delay(3000);
  };
  if (down == LOW) {

    enter_pressed = true;
    if(menu_option[screen_page] >= 0){ //5 is the maximum of options on the page
      menu_option[screen_page]--;
    }


    delay(3000);
  };
  if (enter == LOW) {Serial.println("enter 1");

    enter_pressed = true;
    page_shown_flag = false;

    switch(menu_option[screen_page]){
      case 0:
        if(screen_page < 3){  //3 is the number of pages an option has
          screen_page++;
        }
        break;
      case 1:
        if(screen_page < 4){
          screen_page++;
        }
        break;
      case 2:
        if(screen_page < 2){
          screen_page++;
        }
        break;
      case 3:
        if(screen_page < 2){
          screen_page++;
        }
    }

    /*currentMenuLevel++; // Go back one menu level
    entered = selected;
    enteredTfare = selectedTfare;*/

    //while  (enter == LOW);
    Serial.print("currentMenuLevel: ");
    Serial.println(currentMenuLevel);
    //while(enter == LOW);
    delay(3000);
    Serial.println("enter 2");
  };
  if (back == LOW) { Serial.println("back 1");

    if(screen_page > 0){
      enter_pressed = true;
      screen_page--;
      
    }
    
   
    Serial.print("currentMenuLevel: ");
    Serial.println(currentMenuLevel);
    //while(back == LOW);
    delay(3000);
    Serial.println("back 2");
  }
  

  const char *collectFareOptions[2] = {
    " FIXED PAY ",
    " VARIABLE PAY ",
  };

  switch(screen_page){
    case 0: //option 1 of main menu
      if(enter_pressed == false){
        if(page_shown_flag == false){
          Serial.println("Menulevel-1");
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
          page_shown_flag = true;
        }
      }else{
        switch(menu_option[0]){
          case -1:
            Serial.println("Menulevel-2");
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
              }else if (i != selectedTfare) {
                display.setTextColor(SSD1306_WHITE);
                display.println(collectFareOptions[i]);
              }
            }
            menu_option[0] = 0;
            break;
          case 0: //option 1.1 of main menu
            Serial.println("Menulevel-3A");
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println(F("Ondigo menu>"));
            display.println("Current wallet balance>");
            display.setTextColor(SSD1306_WHITE);
            display.setTextSize(2);
            display.println(" NGN10,000 ");
            break;
          case 1: //option 1.2 of main menu
            Serial.println("Menulevel-3B");
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
        enter_pressed == false;
        page_shown_flag = true;
        display.display();
      }
      break;

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

  Serial.println("Menulevel-1");
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
  
  displaymenu();
}

void loop() {
  // put your main code here, to run repeatedly:
  displaymenu();
}