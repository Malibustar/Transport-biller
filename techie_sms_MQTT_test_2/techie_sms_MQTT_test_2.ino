#define DEBUG true




//******************* MQTT Parameters *******************//

String MQTT_BROKER = "test.mosquitto.org";
String MQTT_PORT = "1883";
String MQTT_USERNAME = "";
String MQTT_PASSWORD = "";


//******************* SIM Paramaters *******************//

String SOS_NUM = "";
String APN_NAME = "internet.ng.airtel.com";


//******************* Data Sending Interval *******************//
uint16_t Send_Data_After = 60; // 60 sec waiting

//******************* Necessary Variables *******************//

String fromGSM = "";
String res = "";
char* response = " ";
String location_data;
String lats;
String longi;
String link1;
String link2;
String msg;

// define two tasks for MQTT_Task & SOSButton_Task
void MQTT_Task( void *pvParameters );
void SOSButton_Task( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup()
{


  Serial.begin(115200); // For ESP32
  Serial2.begin(115200); //For A9G
  

 /* msg = "";
  msg = sendData("AT+RST=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+RST = 1", 2000, DEBUG);
    Serial.println("Trying");
  }
*/
  Serial.println("Before Delay");
  delay(2000);// Waiting For 15 Sec for Initialisation

  Serial.println("After Delay");


 msg = "";
  msg = sendData("AT+CSQ", 1000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CSQ", 1000, DEBUG);
    Serial.println("Trying");
  }
  
  msg = "";
  msg = sendData("AT", 1000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+MQTTDISCONN", 1000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+MQTTDISCONN", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+CGATT=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGATT=1", 1000, DEBUG);
    Serial.println("Trying");
  }

  msg = "";
  msg = sendData("AT+CGDCONT=1,\"IP\",\"" + APN_NAME + "\"", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGDCONT=1,\"IP\",\"" + APN_NAME + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }


  msg = "";
  msg = sendData("AT+CGACT=1", 2000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+CGACT=1", 1000, DEBUG);
    Serial.println("Trying");
  }



  msg = "";
  msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, DEBUG);
  while ( msg.indexOf("OK") == -1 ) {
    msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    MQTT_Task
    ,  "MQTT_Task"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  0);

  xTaskCreatePinnedToCore(
    SOSButton_Task
    ,  "SOSButton_Task"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL
    ,  0);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void MQTT_Task(void *pvParameters)  // This is a task.
{
  (void) pvParameters;


  for (;;) // A Task shall never return or exit.
  {
    
      String coordinates = "0123232" ;
      Serial.print("coordinates - "); Serial.print(coordinates); Serial.println("-");

      link1 = ("AT+MQTTPUB=\"franky\",\"" + coordinates + "\",0,0,0") ;
      //Serial2.println("AT+MQTTPUB=\"A9G\",\"12.34567890123456789.0123456\",0,0,0");
      Serial.print("link lat -"); Serial.println(link1);

      //Serial.print("For Serial Monitor-"); Serial.println(link1);

      // Serial2.println(link1);
      sendData(link1, 1000, DEBUG);

      delay(2000);

      Serial.println("Location DataSend");

    

    msg = "";
    msg = sendData("AT+CBC?", 2000, DEBUG);
    while ( msg.indexOf("OK") == -1 ) {
      msg = sendData("AT+CBC?", 1000, DEBUG);
      Serial.println("Trying");//AT+CBC? //+CBC: 1, 92

    }
    Serial.print("Recevied Data Before - "); Serial.println(msg); // printin the String in lower character form
    int count = 0;
    while (msg[count] != ',')
    {
      count++;
      Serial.print(msg[count]);

    }

    msg = msg.substring(count + 2);

    count = 0;
    while (msg[count] != '\n')
    {
      count++;
      Serial.print(msg[count]);

    }

    msg = msg.substring(0, count - 1);
  

    Serial.print("Recevied Data - "); Serial.println(msg); // printin the String in lower character form
    Serial.println("\n");

    link2 = ("AT+MQTTPUB=\"franky\",\"" + msg + "\",0,0,0") ;
    Serial.print("battery link -"); Serial.println(link2);
    Serial.print("For Serial Monitor-"); Serial.println(link2);

    // Serial2.println(link1);
    msg = "";
    msg =  (sendData(link2, 1000, DEBUG));
    char* msg_char = &msg[0];
    Serial.print("LAT MSG - "); Serial.println(msg_char);
    if ( !(strstr(msg_char, "OK" ))) {
     // MQTT_ReConnect();
    }
    delay(2000);

    Serial.println("Battery DataSend");

    response = "";
    res = "";


    Serial.println("Delay");

    vTaskDelay((Send_Data_After * 1000));
  }
}

void SOSButton_Task(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /*
    AnalogReadSerial
    Reads an analog input on pin A3, prints the result to the serial monitor.
    Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
    Attach the center pin of a potentiometer to pin A3, and the outside pins to +5V and ground.

    This example code is in the public domain.
  */

  for (;;)
  {
  vTaskDelay(10);  // one tick delay (15ms) in between reads for stability
  }
}

String sendData(String command, const int timeout, boolean debug)
{
  String temp = "";
  Serial2.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
      temp += c;
    }
  }
  if (debug)
  {
    Serial.print(temp);
  }
  return temp;
}



void MQTT_ReConnect()
{

  String new_msg = "";
  new_msg = sendData("AT+MQTTDISCONN", 2000, DEBUG);
  while ( new_msg.indexOf("OK") == -1 ) {
    new_msg = sendData("AT+MQTTDISCONN", 2000, DEBUG);
    Serial.println("Trying");
  }


  new_msg = "";
  new_msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, DEBUG);
  while ( new_msg.indexOf("OK") == -1 ) {
    new_msg = sendData("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 1000, DEBUG);
    Serial.println("Trying");
  }


}
