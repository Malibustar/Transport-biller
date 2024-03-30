

void setup() {
  Serial.begin(115200);
 // Set console baud rate
  delay(10);
  serial1.begin(115200); //for A9G Board

  
  pinMode(BL_PIN, OUTPUT);
  pinMode(SCREEN_GND_PIN, OUTPUT);
  pinMode(RFID_GND_PIN, OUTPUT);


  digitalWrite(BL_PIN, HIGH);  // Turn on the backlight initially
  digitalWrite(SCREEN_GND_PIN, LOW);  // Turn on the backlight initially
  digitalWrite(RFID_GND_PIN, LOW);  // Turn on the backlight initially

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  Serial.println("Screen Initialized");


tft.fillScreen(ST77XX_BLACK);
int w = tft.width();
int h = tft.height();
Serial.print("width =");
Serial.println(w);
Serial.print("height =");
Serial.println(h);
//expecting 128 x 160
 tft.setRotation(1); 

Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }

  keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
  char keys[] = "1234567890ABCDE NF";  //  N = NoKey, F = Fail



    serial1.println("AT+CGATT=1"); // Connect to the network
    serial1.println("AT+CGDCONT=1,\"IP\",\"apn\"");    
    serial1.println("AT+CGACT=1,1"); //Activate the gprs (packet data protocal PDP)
    delay(500);

    serial1.println("AT+MQTTCONN=\"test.mosquitto.org\",1883,\"mqttx_0931852d34\",120,0");
    delay(100);
    serial1.println("AT+MQTTSUB=\"/public/TEST/makerfabs-B\",1,0");

    
    //sendData("AT+CIPSTART=\"TCP\",\"www.mirocast.com\",80");
    serial1.println("AT+GPS=1"); //turn on GPS MODULE

// Call the function to get battery level and signal strength
  getSignalAndBattery(batteryLevel, signalStrength);
  // Print the values obtained
  Serial.print("Battery Level: ");
  Serial.println(batteryLevel);
  Serial.print("Signal Strength: ");
  Serial.println(signalStrength);


  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 57); 
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.print("Booting..."); // display "scan now" on device screen
  Serial.println("Booting..."); 


recieveMessage(deviceID, tfare, walletBal, admin);
// Now deviceID, tfare, walletBal, and admin variables hold the parsed string values


 serial1.println("AT+GPSRD=0"); //stop sending GPS data
   // serial.write("AT+GPS=0");

serial1.println("AT+GPSLP = 2"); //GPS Low power


Serial.println("Wait...");

  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  // Enhance the MFRC522 Receiver Gain to maximum value of some 48 dB
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  //Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");

delay(1000);
}
