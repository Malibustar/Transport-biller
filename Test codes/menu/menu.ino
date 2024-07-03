


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "Wire.h"
#include "I2CKeyPad.h"

#define COLOR_BACKGROUND  ST77XX_BLACK
#define COLOR_TEXT        ST77XX_WHITE // White for high readability
#define COLOR_HIGHLIGHT   ST77XX_BLUE  // Blue background for selected item
#define COLOR_SELECTED    ST77XX_CYAN  // Cyan for header and highlighting lines

const int num_items = 7;     // Number of menu items
const int num_subitems = 4;  // Number of subitems for each item
int selected_item = 0;        // Currently selected menu item
int selected_subitem = 0;     // Currently selected subitem
bool in_subitem_page = false; // Flag to indicate if in subitem page
bool in_page = false; // Flag to indicate if in page
int progress = 0;
long lastMsg = 0;
int count = 0;
float buttonTimer = 0;
bool buttonPressed = false;
String pressedCharacter = "";
bool stringAdded = false;
String amountString = "";
String sub_message ="";

const int screenWidth = 360; // Screen width
const int screenHeight = 240; // Screen height



#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18

#elif defined(ESP8266)
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18


#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
#define TFT_CS         2  //case select connect to pin 5
  #define TFT_RST        15 //reset connect to pin 15
  #define TFT_DC         32 //AO connect to pin 32  (not sure that this is really used)  try pin 25
  #define TFT_MOSI       13 //Data = SDA connect to pin 23
  #define TFT_SCLK       12 //Clock = SCK connect to pin 18
  #define TFT_MISO       25 //Clock = SCK connect to pin 18

#endif

#define BL_PIN 26
#define LED 33

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



// Menu items
const char* menu_items[num_items] = {
  "Item 1",
  "Item 2",
  "Item 3",
  "Item 4",
  "Item 5",
  "Item 6",
  "Item 7"
};

// Subitems for each menu item
const char* subitems[num_items][num_subitems] = {
  {"Subitem 1-1", "Subitem 1-2", "Subitem 1-3", "Subitem 1-4"},
  {"Subitem 2-1", "Subitem 2-2", "Subitem 2-3", "Subitem 2-4"},
  {"Subitem 3-1", "Subitem 3-2", "Subitem 3-3", "Subitem 3-4"},
  {"Subitem 4-1", "Subitem 4-2", "Subitem 4-3", "Subitem 4-4"},
  {"Subitem 5-1", "Subitem 5-2", "Subitem 5-3", "Subitem 5-4"},
  {"Subitem 6-1", "Subitem 6-2", "Subitem 6-3", "Subitem 6-4"},
  {"Subitem 7-1", "Subitem 7-2", "Subitem 7-3", "Subitem 7-4"}
};

// Pages for each subitem
const char* pages[num_items][num_subitems] = {
  {"Page 1-1", "Page 1-2", "Page 1-3", "Page 1-4"},
  {"Page 2-1", "Page 2-2", "Page 2-3", "Page 2-4"},
  {"Page 3-1", "Page 3-2", "Page 3-3", "Page 3-4"},
  {"Page 4-1", "Page 4-2", "Page 4-3", "Page 4-4"},
  {"Page 5-1", "Page 5-2", "Page 5-3", "Page 5-4"},
  {"Page 6-1", "Page 6-2", "Page 6-3", "Page 6-4"},
  {"Page 7-1", "Page 7-2", "Page 7-3", "Page 7-4"}
};

// Function to display the menu
void displayMenu() {
    tft.fillScreen(COLOR_BACKGROUND); // Clear the screen with the background color

    // Set the text size and color for the header
    tft.setTextSize(3); // Increase text size for the header
    tft.setTextColor(COLOR_SELECTED); // Use a color that stands out for the header

    // Set the cursor to the top of the screen and print the "Menu" title
    tft.setCursor(10, 10); // Adjust cursor position as needed for the header
    tft.println(F("Menu"));

    // Draw a horizontal line under the header for visual separation
    tft.drawFastHLine(0, 45, tft.width(), COLOR_SELECTED); // Adjust Y position to fit the larger header

    // Set text size for menu items
    tft.setTextSize(2); // Increased text size for menu items for better readability

    for (int i = 0; i < num_items; i++) {
        int y = 55 + i * 25; // Adjust Y position to start below the header, with increased spacing for larger text

        // Highlight the background of the selected item
        if (i == selected_item) {
            // Ensure there is high contrast between the highlighted background and text
            tft.fillRect(0, y, tft.width(), 24, COLOR_HIGHLIGHT); // Adjust height for the larger text
            tft.setTextColor(COLOR_TEXT, COLOR_HIGHLIGHT); // White text on blue background
        } else {
            tft.setTextColor(COLOR_TEXT, COLOR_BACKGROUND); // Non-selected items
        }

        // Set cursor and print each menu item
        tft.setCursor(10, y + 4); // Adjust cursor X position, Y adjusted for item spacing and font size
        tft.println(menu_items[i]);
    }
}

// Function to display subitems for the selected menu item
void displaySubitems() {
    tft.fillScreen(COLOR_BACKGROUND); // Clear the screen with the background color

    // Set the text size and color for the submenu header
    tft.setTextSize(3); // Increased text size for the header for better visibility
    tft.setTextColor(COLOR_SELECTED); // Use the selected color for the header text

    // Set the cursor to the top of the screen and print the "Sub-Menu" title
    tft.setCursor(10, 10); // Adjust cursor position as needed
    tft.println(F("Sub-Menu"));

    // Draw a horizontal line under the header for visual separation
    tft.drawFastHLine(0, 45, tft.width(), COLOR_SELECTED); // Adjust Y position to fit the larger header text

    // Increase the text size for the submenu items
    tft.setTextSize(2); // Larger text size for submenu items for readability

    for (int i = 0; i < num_subitems; i++) {
        int y = 55 + i * 25; // Adjust Y position for increased spacing due to larger text size

        // Highlight the background of the selected submenu item for better visibility
        if (i == selected_subitem) {
            tft.fillRect(0, y, tft.width(), 24, COLOR_HIGHLIGHT); // Highlight background with a solid color
            tft.setTextColor(COLOR_TEXT, COLOR_HIGHLIGHT); // Text color for selected item (high contrast)
        } else {
            tft.setTextColor(COLOR_TEXT, COLOR_BACKGROUND); // Non-selected items' text color
        }

        // Set cursor and print each submenu item
        tft.setCursor(10, y + 4); // Adjust cursor X position, Y adjusted for item spacing and text size
        tft.println(subitems[selected_item][i]);
    }
}

// Function to display the page for the selected subitem
void displayPage() {
    tft.fillScreen(COLOR_BACKGROUND); // Clear the screen with the background color

    // Dynamic page title based on the selected submenu item
    String pageTitle = pages[selected_item][selected_subitem];

    // Set the text size and color for the page header
    tft.setTextSize(3); // Increased text size for the header
    tft.setTextColor(COLOR_SELECTED); // Use the selected color for the header text

    // Set the cursor to the top of the screen and print the page title
    tft.setCursor(10, 10); // Adjust cursor position as needed
    tft.println(pageTitle);

    // Draw a horizontal line under the header for visual separation
    tft.drawFastHLine(0, 45, tft.width(), COLOR_SELECTED); // Adjust the Y position to fit the larger header text

    // Set the text size for the page content
    tft.setTextSize(2); // Text size for page content for readability

    // Display the page content below the header
    // Here, we're simply repeating the page title for demonstration,
    // but you could expand this to show any content relevant to the page
    int contentY = 55; // Y position to start the content, below the header
    tft.setCursor(10, contentY);
    tft.setTextColor(COLOR_TEXT); // Text color for content
    tft.println(pageTitle); // For demonstration, repeat the title as content

    // Optionally, add more page content here
    // tft.setCursor(10, contentY + 25); // Adjust for next line of content
    // tft.println("Additional content here");
}

// Function to handle scrolling up
void scrollUp() {
    if (in_page)
      return;

  if (!in_subitem_page) {
    if (selected_item > 0 && selected_subitem == 0) {
      selected_item--;
      displayMenu();
    } else if (selected_item <= 0 && selected_subitem == 0) {
    selected_item = num_items -1;
    displayMenu();
    }
  }
  else{
     if (selected_subitem > 0 ) {
   selected_subitem--;
         displaySubitems();
    } else if (selected_subitem <= 0 ) {
selected_subitem = num_subitems - 1;
    displaySubitems();
    }
  }
}

// Function to handle scrolling down
void scrollDown() {
  if (in_page)
      return;

  if (!in_subitem_page) {
    //main menu
    if (selected_item < num_items - 1 && selected_subitem == 0) {
      selected_item++;
      displayMenu();
    }
    else if (selected_item >= num_items -1 && selected_subitem == 0 ){
      selected_item = 0;
        displayMenu();
    } 
  }
  else{
     if (selected_subitem < num_subitems - 1) {
      selected_subitem++;
      displaySubitems();
    }
    else if (selected_subitem >= num_subitems -1 ) {
      selected_subitem = 0;
      // If the last item is selected, but not the last subitem, move to the next subitem
     // selected_subitem++;
     displaySubitems();
    }
  }
}


// Function to handle item selection
void selectItem() {
  if (in_subitem_page) {
    // displaySubitems();
    displayPage();
    in_page = true;
  } else {
    if (selected_subitem == 0) {
      // Display subitems for the selected menu item
          in_subitem_page = true;
     displaySubitems();
 
    } else {
      // Display the page for the selected subitem
     // in_subitem_page = true;
      //displayPage();
    }
  }
}

// Function to handle going back
void goBack() {

  if (in_subitem_page && in_page) {
    // Return to subitem selection
    in_page = false;
    displaySubitems();
  } else {
    // Return to main menu
    // selected_item = 0;
    selected_subitem = 0;
    in_subitem_page = false;
    displayMenu();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 
   Wire.begin();
  Wire.setClock(400000L);

  pinMode(BL_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  delay(100);
 digitalWrite(BL_PIN, HIGH);
 digitalWrite(LED, HIGH); 
  
    // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  tft.setRotation(1);
  tft.invertDisplay(false);
  


tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,ST77XX_WHITE); //Draw white frame

displayMenu(); 
}

void loop(void) {
  
 readKeyPad();

if (buttonPressed == true && pressedCharacter == "2")
  {
    scrollUp();
  }
  
  // Check if buttons are pressed
 
if (buttonPressed == true && pressedCharacter == "8")
  {
    scrollDown();
  }

if (buttonPressed == true && pressedCharacter == "4")
  {
    goBack();
  }


  if (buttonPressed == true && pressedCharacter == "5") {
    selectItem();
  }
}

void readKeyPad()
{
  if (buttonPressed == true)
  {
    if (millis() - buttonTimer > 300)
    {
      //The button pressed is only set back to false after 300 ms, so we cannot press a button twice quickly
      buttonTimer = millis();
      buttonPressed = false;
    }
    else
    {
      //do nothing
    }
  }
  else
  {
    //B11101111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B11101111);
    //[P7]B11101111[P0] -> [P7]1110[P4] - activates first row, [P3]1111[P0] - Sets all pins high on the MUX
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      //11101110 - P0 pin went low after pressing the button -> 1 was pressed
      case 238: //Button 1
        Serial.print("1");
        pressedCharacter = "1";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101101 - P1 pin went low after pressing the button -> 2 was pressed
      case 237: //Button 2
        Serial.print("4");
        pressedCharacter = "4";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11101011 - P2 pin went low after pressing the button -> 3 was pressed
      case 235: //Button 3
        Serial.print("7");
        pressedCharacter = "7";
        buttonPressed = true;
        stringAdded = true;
        break;

      //11100111 - P3 pin went low after pressing the button -> A was pressed
      case 231: //Button A
        Serial.println("*");
        pressedCharacter = "*";
        buttonPressed = true;
        break;
    }
    //-------------------------------------------
    //B11011111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B11011111);
    //[P7]B11011111[P0] -> [P7]1101[P4] - activates second row, [P3]1111[P0] - Sets all pins high on the MUX
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      //11011110 - P0 pin went low after pressing the button -> 2 was pressed
      case 222: //Button 4
        Serial.print("2");
        pressedCharacter = "2";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 221: //Button 5
        Serial.print("5");
        pressedCharacter = "5";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 219: //Button 6
        Serial.print("8");
        pressedCharacter = "8";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 215: //Button B
        Serial.println("0");
        pressedCharacter = "0";
        buttonPressed = true;
        stringAdded = true;
        break;
    }
    //-------------------------------------------
    //B10111111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B10111111);
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      case 190: //Button 7
        Serial.print("3");
        pressedCharacter = "3";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 189: //Button 8
        Serial.print("6");
        pressedCharacter = "6";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 187: //Button 9
        Serial.print("9");
        pressedCharacter = "9";
        buttonPressed = true;
        stringAdded = true;
        break;

      case 183: //Button C
        Serial.println("#");
        pressedCharacter = "#";
        buttonPressed = true;
        break;
    }
    //-------------------------------------------
    //B01111111
    Wire.beginTransmission(0x20); //00100000
    Wire.write(B01111111);
    Wire.endTransmission();

    Wire.requestFrom(0x20, 1);
    switch (Wire.read())
    {
      case 126: //Button *
        Serial.print("A");
        pressedCharacter = "A";
        buttonPressed = true;
        break;

      case 125: //Button 0
        Serial.print("B");
        pressedCharacter = "B";
        buttonPressed = true;
        break;

      case 123: //Button #
        Serial.print("C");
        pressedCharacter = "C";
        buttonPressed = true;
        break;

      case 119: //Button D
        Serial.println("D");
        pressedCharacter = "D";
        buttonPressed = true;
        break;
    }
    buttonTimer = millis();
  }
}

// Function to erase highlight for a subitem
void eraseHighlight(int subitem) {
  int textHeight = 20;
  int spacing = (screenHeight - num_subitems * textHeight) / (num_subitems + 1);
  int yPos = spacing * (subitem + 1) + textHeight * subitem;
  tft.fillRect(0, yPos - spacing, screenWidth, textHeight + spacing, ST77XX_BLACK);
}

// Function to highlight a subitem
void highlightSubitem(int subitem) {
  int textHeight = 20;
  int spacing = (screenHeight - num_subitems * textHeight) / (num_subitems + 1);
  int yPos = spacing * (subitem + 1) + textHeight * subitem;
  tft.fillRect(0, yPos - spacing, screenWidth, textHeight + spacing, ST77XX_BLUE);
}

void drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
    }
  }
}
