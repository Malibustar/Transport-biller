
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY     0xD6BA

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

//#define BL_PIN 26
#define LED 33

// OR for the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

extern uint8_t logo_round_rect[];
extern uint8_t  go_logo[];
extern uint8_t  battery_17[];
extern uint8_t  battery_33[];
extern uint8_t  battery_50[];
extern uint8_t  battery_67[];
extern uint8_t  battery_83[];
extern uint8_t  battery_full[];
extern uint8_t  charger_connected[];
extern uint8_t  network_1_bar[];
extern uint8_t  network_2_bars[];
extern uint8_t  network_3_bars[];
extern uint8_t  network_4_bars[];
extern uint8_t  network_not_connected[];
extern uint8_t  internet[];
extern uint8_t location_icon[];

int progress = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 

  //pinMode(BL_PIN, OUTPUT);
  pinMode(LED, OUTPUT);
  delay(100);
 //digitalWrite(BL_PIN, HIGH);
 digitalWrite(LED, HIGH); 
  
    // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240

  tft.setRotation(1);
  tft.invertDisplay(false);
  


tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,WHITE); //Draw white frame
}

void loop(void) {
  
 
  tft.drawBitmap(127, 89, logo_round_rect, 63, 49, ST77XX_WHITE);
tft.drawBitmap(132, 95, go_logo, 51, 36, ST77XX_WHITE);
tft.drawBitmap(275, 124, battery_17, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 104, battery_33, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 82, battery_50, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 60, battery_67, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 38, battery_83, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 17, battery_full, 24, 16, ST77XX_WHITE);
tft.drawBitmap(275, 147, charger_connected, 24, 16, ST77XX_WHITE);
tft.drawBitmap(19, 86, network_1_bar, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 61, network_2_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 38, network_3_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 16, network_4_bars, 15, 16, ST77XX_WHITE);
tft.drawBitmap(19, 110, network_not_connected, 15, 16, ST77XX_WHITE);
tft.drawBitmap(48, 15, internet, 15, 16, ST77XX_WHITE);
tft.drawBitmap(79, 14,location_icon, 13, 16, ST77XX_WHITE);
//tft.drawRect(x, y, w, h, colour);
tft.drawRect(71, 201, 171, 2, ST77XX_WHITE);
tft.drawRect(71, 201, progress, 2, ST77XX_GREEN);



progress = progress + 10;
if (progress > 171) {
progress = 0;
}


 
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
