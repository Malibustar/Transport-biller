

void setup() {
  Serial.begin(115200);
 // Set console baud rate
  delay(10);
  serial1.begin(115200); //for A9G Board

  
  pinMode(BL_PIN, OUTPUT);
  pinMode(SCREEN_GND_PIN, OUTPUT);
  pinMode(RFID_GND_PIN, OUTPUT);

  delay(100);  
  digitalWrite(BL_PIN, HIGH);  // Turn on the backlight initially
  digitalWrite(SCREEN_GND_PIN, HIGH);  // Turn on the backlight initially
  digitalWrite(RFID_GND_PIN, HIGH);  // Turn on the backlight initially


    // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  tft.init(240, 320);           // Init ST7789 320x240
  tft.setRotation(1);
  tft.invertDisplay(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.drawRect(0,0,320,240,WHITE); //Draw white frame


tft.drawBitmap(127, 89, logo_round_rect, 63, 49, ST77XX_WHITE);
tft.drawBitmap(132, 95, go_logo, 51, 36, ST77XX_WHITE);
//tft.drawRect(x, y, w, h, colour);
tft.drawRect(71, 201, 171, 2, ST77XX_WHITE);
tft.drawRect(71, 201, progress, 2, ST77XX_GREEN);

progress = progress + 1;
if (progress > 171) {
progress = 0;

}
}