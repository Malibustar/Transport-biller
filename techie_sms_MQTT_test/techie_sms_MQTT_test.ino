#define DEBUG true




//******************* MQTT Parameters *******************//

String MQTT_BROKER = "broker.hivemq.com";
String MQTT_PORT = "1883";
String MQTT_USERNAME = "";
String MQTT_PASSWORD = "";

String IPAddress = "3.120.58.229";
String IPPort = "1883";


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

  //if (!attemptCommand("AT+MQTTDISCONN", 1000, 5, "OK")) return;

    if (!attemptCommand("AT+CGATT=1", 1000, 5, "OK")) return;

    if (!attemptCommand("AT+CGDCONT=1,\"IP\", \"CMNET\"", 1000, 2, "OK")) {
      Serial.println("Something happened with IP - CMNET. Proceeding anyways");
      
      }
  

    if (!attemptCommand("AT+CGACT=1,1", 1000, 5, "OK")) return;


 // if (!attemptCommand("AT+CIPSTART=\"TCP\",\"" + IPAddress + "\"," + IPPort, 1000, 5, "OK")) return;

 if (!attemptCommand("AT+MQTTDISCONN", 1000, 5, "OK")) return;
if (!attemptCommand("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"mqttx_0931852d35\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, 10, "OK")) return;
 
  //AT+CIPSTART="TCP","3.120.58.229",1883

  if (!sendMQTTPublishCommand("franky", "12345")) return;
// if (!attemptCommand("AT", 1000, 5, "OK")) return;
//   if (!attemptCommand("AT+MQTTDISCONN", 1000, 5, "OK")) return;

//  // Check if GPRS is attached, and only proceed to attach if it's not.
//   String cgattResponse = sendData("AT+CGATT?", 2000, DEBUG);
  
//   if (cgattResponse.indexOf("+CGATT: 0") != -1) {
//     if (!attemptCommand("AT+CGATT=1", 2000, 5, "OK")) { 
//       Serial.println("GPRS Attachment failed...");
//       return;
//      // return to stop return;
//     }
//   } else if (cgattResponse.indexOf("+CGATT: 1") == -1) {
//     Serial.println("Unexpected GPRS attachment status. Proceeding");
//   //  return;
//   }

//   // Check if the correct APN is already set, and set it if not.
//   String cgdcontResponse = sendData("AT+CGDCONT?", 2000, DEBUG);
  
//   if (cgdcontResponse.indexOf("\"" + APN_NAME + "\"") == -1) {
//     if (!attemptCommand("AT+CGDCONT=1,\"IP\",\"" + APN_NAME + "\"", 2000, 5, "OK")) {
//       Serial.println("APN Configuration not configured. Proceeding...");
//       }
//   }

//   if (!attemptCommand("AT+CGATT=1", 2000, 5, "OK")) return;

// //  if (!attemptCommand("AT+MQTTCONN=\"" + MQTT_BROKER + "\"," + MQTT_PORT + ",\"ABCD\",120,0,\"" + MQTT_USERNAME + "\",\"" + MQTT_PASSWORD + "\"", 3000, 1, "OK")) return;
//   if (AttemptMQTTConnection(10)== "0") return;
  
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
      Serial.println("Trying");
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

bool attemptCommand(String command, int timeout, int attempts, String successIndicator) {
  for (int tryCount = 0; tryCount < attempts; tryCount++) {
    String response = sendData(command, timeout, DEBUG);
    if (response.indexOf(successIndicator) != -1) {
      return true; // Success
    }
    delay(1000); // Wait a bit before retrying
  }
  Serial.println("Failed to execute command: " + command);
  return false; // Failed after all attempts
}
void MQTT_ReConnect() {
    // Make sure the PDP context is active
    if (sendData("AT+CGACT?", 2000, DEBUG).indexOf("+CGACT: 1,1") == -1) {
        if (!attemptCommand("AT+CGACT=1,1", 10000, 5, "OK")) {
            Serial.println("PDP context activation failed. Retrying...");
            // Optionally, deactivate and reactivate PDP context here
        }
    }

    // Retry MQTT connection
    if (AttemptMQTTConnection(5)== "0") return;
}


String AttemptMQTTConnection(int retry){

    for (int attempts = 0; attempts < retry; attempts++) {
        String mqttConnResponse = sendData("AT+MQTTCONN=\"broker.hivemq.com\",1883,\"ABCD\",120,0,\"\",\"\"", 10000, DEBUG);
        if (mqttConnResponse.indexOf("OK") != -1) {
            Serial.println("MQTT Connected Successfully.");
            return "OK"; // Success, exit the retry loop
        } else if (mqttConnResponse.indexOf("+CME ERROR: 50") != -1) {
            Serial.println("Insufficient network service, retrying MQTT connection...");
            delay(5000); // Wait before retrying to allow for network conditions to change
        }
    }
    Serial.println("Failed to establish MQTT connection after multiple attempts.");
    return "0";
}
bool sendMQTTPublishCommand(String topic, String message) {
    // Initiate the send operation
    Serial2.println("AT+CIPSEND");
    unsigned long startTime = millis();
    bool promptReceived = false;

    // Wait for '>' prompt indicating the module is ready for data input
    while (millis() - startTime < 5000) { // 5-second timeout
        if (Serial2.available()) {
            char c = Serial2.read();
            if (c == '>') {
                promptReceived = true;
                break; // Exit the loop once the prompt is received
            }
        }
    }

    if (!promptReceived) {
        Serial.println("Prompt not received for CIPSEND.");
        return false; // Exit the function if prompt wasn't received
    }

    // Send the MQTT publish command followed by a termination sequence (CTRL+Z / 0x1A)
    String pubCommand = "AT+MQTTPUB=\"" + topic + "\",\"" + message + "\",0,0,0";
    Serial2.print(pubCommand); // Send the publish command
    Serial2.write(0x1A); // CTRL+Z to terminate the send operation

    // Wait and read the response
    String response = "";
    startTime = millis();
    while (millis() - startTime < 10000) { // 10-second timeout for the response
        while (Serial2.available()) {
            char c = Serial2.read();
            response += c;
            if (response.indexOf("OK") != -1 || response.indexOf("ERROR") != -1) {
                break; // Break the loop if "OK" or "ERROR" is found
            }
        }
    }

    // Log the full response for debugging
    if(DEBUG) {
        Serial.println(response);
    }

    // Check response for success or error
    if (response.indexOf("OK") != -1) {
        return true; // Data sent successfully
    } else {
        Serial.println("Data send failed or no confirmation received.");
        return false; // Send operation failed or no clear response was received
    }
}
