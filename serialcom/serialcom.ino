//******* Pin Configurations *******//
/*
#include <Arduino.h>


void setup() {
  // initialize both serial ports:
  Serial.begin(115200);// For ESP32
  Serial2.begin(115200); //For A9G



}

void loop()
{
  // read from port 1, send to port 0:
  if (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial2.write(inByte);
  }
}
*/



#include <string.h>
#include <math.h>

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

/*#define APN_STRING "Judroid"
#define PSW_STRING "12Ju34da65h"*/

#define ON        1
#define OFF       0
#define NETWORK_TIMEOUT     300
//#define IOT_STATUS_ON       (PORTB |= (1<<PINB0))
//#define IOT_STATUS_OFF      (PORTB &= ~(1<<PINB0))

unsigned char received_data[25];
signed char strIndex;

volatile uint8_t time_zone = 1;

volatile unsigned char time_characters[13];

volatile bool receiving_from_server,server_response,connection_to_server,abruptly_closed_connection,network_registration_flag;

unsigned char present_month,present_day,present_hour,present_week;

unsigned int TCP_character_count;

void disconnect_device_at_server(void);
void register_device_at_server(void);

void setup_SFR(void){
  SREG = 0x80;
  
  //TCCR0 = TCCR0 |(1<<CS00);
  TIMSK0 = TIMSK0 |(1<<TOIE0);
  //TIMSK = TIMSK |(1<<TOIE0);
  
  UCSR0A = UCSR0A |(1<<U2X0);
  //UCSRA = UCSRA |(1<<U2X);
  UCSR0B = UCSR0B |(1<<RXEN0)|(1<<TXEN0);
  //UCSRB = UCSRB |(1<<RXEN)|(1<<TXEN);
  UCSR0C = UCSR0C |(1<<UCSZ01)|(1<<UCSZ00);
  //UCSRC = UCSRC |(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
  UBRR0H = 0x00;
  //UBRRH = 0x00;
//  UBRRL = 12; //9600bps at 1MHz
//  UBRRL = 103;  //9600bps at 8MHz
  UBRR0L = 207; //9600bps at 16MHz
  //UBRRL = 207;  //9600bps at 16MHz
}

void wait_for_response(void){
  _delay_ms(1500);
}

void USART_send_data(unsigned char var1){
  //_delay_ms(5);
  /* Wait for empty transmit buffer */
  //while ( !( UCSRA & (1<<UDRE)) );
  while ( !( UCSR0A & (1<<UDRE0)) );
  /* Put data into buffer, sends the data */
  if(var1!=0){
    TCP_character_count++;
    //UDR = var1;
    UDR0 = var1;
  }
}

void USART_send_string(unsigned char  *var1){
  while(*var1)              //till string ends
    USART_send_data(*var1++);  //send characters one by one
}

void StartTCP(void){
  USART_send_string("AT+CIPSTART=\"TCP\",\"18.185.72.169\",8259\r\n");
}

//void start_WAN_TCP_client(bool wireless_mode, char* APN_detail, char* ISP_username, char* ISP_password){ //GSM = 0 or WiFi = 1
void start_WAN_TCP_client(char* APN_detail, char* ISP_username, char* ISP_password){ //GSM = 0 or WiFi = 1
  //********* for GSM module ****************
  USART_send_string("AT\r\n");
  _delay_ms(2000);
  
//  if(wireless_mode == true){
    //********* for GSM module ****************
    USART_send_string("AT+CIPSHUT\r\n");

    //_delay_ms(1000);
    _delay_ms(1000);
    USART_send_string("AT+CIPMUX=0\r\n");
    //_delay_ms(1000);

    _delay_ms(1000);
    USART_send_string("AT+CGATT=1\r\n");
    //_delay_ms(2000);
    _delay_ms(1000);

    _delay_ms(1000);
    USART_send_string("AT+CSTT=\"");

    //*** APN ***

    //USART_send_string("etisalat");
    USART_send_string(APN_detail);
    //***********

    USART_send_string("\",\"");


    //*** Username ***
    USART_send_string(ISP_username);
    //***********


    USART_send_string("\",\"");

    //*** Password ***

    USART_send_string(ISP_password);
    //***********

    USART_send_string("\"\r\n");

    //_delay_ms(1000);
    _delay_ms(1000);
    USART_send_string("AT+CIICR\r\n");
    _delay_ms(3000);
    USART_send_string("AT+CIFSR\r\n");
    _delay_ms(2000);
    
//  }
  
  //_delay_ms(2000);
  StartTCP();
}

void wait_for_server_connection(void){//myprintf("\nyes_2");

  unsigned int response_loop_count = 0;
  while(connection_to_server == false){
    _delay_ms(100);
    response_loop_count++;
    if(response_loop_count >= NETWORK_TIMEOUT){//myprintf("\nyes_3");
      start_WAN_TCP_client();
      response_loop_count = 0;
    }
  }

  disconnect_device_at_server();
  register_device_at_server();

  connection_to_server = false;
  response_loop_count = 0;
}

void tcp_send_command(void){
  
  USART_send_string("AT+CIPSEND\r\n");
}

void tcp_terminating_characters(void){
  
  //******* (14) ***
  USART_send_string("|~\r\n");
  //if(connection_type == true)
    USART_send_data(0x1A);
}

void ThingsWeb_POP_command(void){
  USART_send_string("/RQST|");
}

void ThingsWeb_PUSH_command(void){
  USART_send_string("/WFTF|");
}

void send_data_to_server(char* operator_ID, char* data_name, char* data_tag, char* data_stream, char* project_name, char* project_ID, char* data_structure){

  SendDataToStart:
  abruptly_closed_connection = false;
  server_response = false;
  tcp_send_command();
  wait_for_response();
  TCP_character_count = 0;
  ThingsWeb_PUSH_command();
  USART_send_string(operator_ID);
  USART_send_data('|');
  USART_send_string(data_name);
  USART_send_data('|');
  USART_send_string(data_tag);
  USART_send_data('|');
  USART_send_data(data_stream);
  USART_send_data('#');
  
  //******* Time (9) ************   //Note: 9 characters for this part
  USART_send_data(time_characters[5]);
  USART_send_data(time_characters[4]);
  USART_send_data(':');
  USART_send_data(time_characters[3]);
  USART_send_data(time_characters[2]);
  USART_send_data(':');
  USART_send_data(time_characters[1]);
  USART_send_data(time_characters[0]);
  USART_send_data('#');

  //******* Date (9) ************   //Note: 9 characters for this part
  USART_send_data(time_characters[11]);
  USART_send_data(time_characters[10]);
  USART_send_data('/');
  USART_send_data(time_characters[9]);
  USART_send_data(time_characters[8]);
  USART_send_data('/');
  USART_send_data(time_characters[7]);
  USART_send_data(time_characters[6]);
  USART_send_data('#');

  //******* folders (68) *********
  
  
  
  
  
  
  
  USART_send_data('|');
  USART_send_string(project_name);
  USART_send_data('|');
  USART_send_string(project_ID);
  USART_send_data('|');
  USART_send_string(data_structure);

/*  if(connection_type == false){
    ///******** for WiFi module ***********
    while(TCP_character_count < 278){//284){
      USART_send_data('*');
    }
    //*************************************
  }*/

  //******* ending character (4) ***
  USART_send_tcp_terminating_characters();

  unsigned int response_loop_count = 0;
  while(server_response == false){
    if(abruptly_closed_connection == true){
      break;
    }
    _delay_ms(100);
    response_loop_count++;
    if(response_loop_count >= NETWORK_TIMEOUT){
      break;
    }
  }
  server_response = false;

  if(response_loop_count >= NETWORK_TIMEOUT){
    IOT_STATUS_OFF;
    //StartTCP();
    wait_for_server_connection();
    goto SendDataToStart;
  }

  if(abruptly_closed_connection == true){
    wait_for_server_connection();
    goto SendDataToStart;
  }

  _delay_ms(500);
}

void request_user_data_command(char* operator_ID, char* data_name, char* project_name, char* project_ID, char* data_save_structure){

  RequestUserData:
  abruptly_closed_connection = false;
  server_response = false;
  tcp_send_command();
  wait_for_response();
  TCP_character_count = 0;
  ThingsWeb_POP_command();  //RQST|Wisol_user|WindSolarLogger_PROJECT_1234|VI|***|~
  USART_send_string(operator_ID);
  USART_send_data('|');
  USART_send_string(data_name);
  USART_send_data('|');
  USART_send_string(project_name);
  USART_send_data('|');
  USART_send_string(project_ID);
  USART_send_data('|');
  USART_send_string(data_save_structure);
  USART_send_data('|');

  ///******** for WiFi module ***********
/*  if(connection_type == false){
    while(TCP_character_count<67){
      USART_send_data('*');
    }*/
//  }
  //*************************************

  //******* ending character (4) ***
  tcp_terminating_characters();

  unsigned int response_loop_count = 0;
  while(server_response == false){
    if(abruptly_closed_connection == true){
      break;
    }//USART_send_string("\r\nJ1");
    _delay_ms(50);//USART_send_string("\r\nJ2");
    response_loop_count++;
    if(response_loop_count >= NETWORK_TIMEOUT){//USART_send_string("\nJ3a\n");
      break;
    }
  }//USART_send_string("\nJ3b\n");
  server_response = false;

  if(response_loop_count >= NETWORK_TIMEOUT){//USART_send_string("\nJ4a\n");
    IOT_STATUS_OFF;
    //StartTCP();
    wait_for_server_connection();//USART_send_string("\nJ4b\n");
    goto RequestUserData;
  }//USART_send_string("\nJ5\n");

  if(abruptly_closed_connection == true){//USART_send_string("\nJ6\n");
    wait_for_server_connection();
    goto RequestUserData;
  }
  //USART_send_string("\nJ7\n");
  _delay_ms(1000);//USART_send_string("\r\nJ8");
}

void disconnect_device_at_server(char* operator_ID, char* project_name, char* project_ID){

  tcp_send_command();
  wait_for_response();
  TCP_character_count = 0;
  USART_send_string("/DISCONNECT");
  USART_send_data('|');
  USART_send_string(project_name);
  USART_send_data('|');
  USART_send_string(project_ID);
  USART_send_data('|');
  USART_send_string(operator_ID);
  USART_send_data('|');

  //USART_send_string("|3|");

//  if(connection_type == false){
    //******** for WiFi module ***********
    /*while(TCP_character_count<49){
      USART_send_data('*');
    }*/
    //*************************************
//  }

  USART_send_tcp_terminating_characters();

  _delay_ms(4000);
}

void register_device_at_server(char* operator_ID, char* project_name, char* project_ID){

  RegisterATServer:
  abruptly_closed_connection = false;
  server_response = false;
  tcp_send_command();
  wait_for_response();
  TCP_character_count = 0;
  USART_send_string("/CONNECT");
  USART_send_data('|');
  USART_send_string(project_name);
  USART_send_data('|');
  USART_send_string(project_ID);
  USART_send_data('|');
  USART_send_string(operator_ID);
  USART_send_string("|3|");
  USART_send_tcp_terminating_characters();

  unsigned int response_loop_count = 0;
  while(server_response == false){
    if(abruptly_closed_connection == true){
      break;
    }
    _delay_ms(100);
    response_loop_count++;
    if(response_loop_count >= NETWORK_TIMEOUT){
      break;
    }
  }
  server_response = false;

  if(response_loop_count >= NETWORK_TIMEOUT){
    IOT_STATUS_OFF;
    //StartTCP();
    wait_for_server_connection();
    goto RegisterATServer;
  }

  if(abruptly_closed_connection == true){
    wait_for_server_connection();
    goto RegisterATServer;
  }

  _delay_ms(500);
}

void requestDateAndTime(char* operator_ID, char* project_name, char* project_ID){

  DateTimeRequest:
  abruptly_closed_connection = false;
  server_response = false;
  tcp_send_command();
  wait_for_response();
  TCP_character_count = 0;
  USART_send_string("/RDT|"); //Request Date and Time
  USART_send_string(operator_ID);
  USART_send_data('|');
  USART_send_string(project_name);
  USART_send_data('|');
  USART_send_string(project_ID);
  USART_send_data('|');

//  if(connection_type == false){
  /*  //******** for WiFi module ***********
    while(TCP_character_count < 42){
      USART_send_data('*');
    }*/
    //*************************************//
//  }

  //******* ending character (4) ***
  USART_send_tcp_terminating_characters();

  unsigned int response_loop_count = 0;
  while(server_response == false){
    if(abruptly_closed_connection == true){
      break;
    }
    _delay_ms(100);
    response_loop_count++;
    if(response_loop_count >= NETWORK_TIMEOUT){
      break;
    }
  }

  server_response = false;

  if(response_loop_count >= NETWORK_TIMEOUT){
    IOT_STATUS_OFF;
    //StartTCP();
    wait_for_server_connection();
    goto DateTimeRequest;
  }

  if(abruptly_closed_connection == true){
    wait_for_server_connection();
    goto DateTimeRequest;
  }

  _delay_ms(500);
}

void initialize_GSM_module(void){
  USART_send_string("ATE0\r\n");  //Off echo
  _delay_ms(500);
  USART_send_string("AT+CMGF=1\r\n"); //Text format
  _delay_ms(500);
  USART_send_string("AT\r\n");
  _delay_ms(500);
  //sms_enabled = 1;
  for(char digit3='1';digit3<='9';digit3++){
    USART_send_string("AT+CMGR=");
    USART_send_data(digit3);
    USART_send_string("\r\n");
    _delay_ms(500);
  }
  USART_send_string("AT+CMGD=1,4\r\n"); //Delete all SMS
  _delay_ms(500);
  //sms_enabled = 0;
}

void initialize_reading_variables(void){
  strIndex = 0;
  receiving_from_server = false;
  //strcpy(received_data,"         ");
  for(int i=0;i<35;i++)
    received_data[i] = 0;
}

void time_and_date_query(void){
  _delay_ms(500);
  USART_send_string("AT\r\n");//USART_send_string("Ju\r");
  _delay_ms(500);//wait_for_response();
  USART_send_string("AT+CCLK?\r\n");
  _delay_ms(500);
}

void thingsweb_rxd_action(void){
  static uint16_t digit1 = 0;
  static uint8_t section_count = 0, received_data_holder[10];//,character_count=0;
  static signed char time_zone = 0;

  /****** wifi response exclude ********/
  /*if((received_data[0]=='+')&(received_data[1]=='I')&(received_data[2]=='P')&(received_data[3]=='D')&(received_data[strIndex]==':')){
    strIndex = -1;
  }*/

  //********* for GSM mode **********
  if(((received_data[0]=='C')&(received_data[1]=='O')&(received_data[2]=='N')&(received_data[3]=='N')&(received_data[4]=='E')&(received_data[5]=='C')&(received_data[6]=='T')&(received_data[7]==' ')&(received_data[8]=='O')&(received_data[9]=='K'))
  |((received_data[0]=='A')&(received_data[1]=='L')&(received_data[2]=='R')&(received_data[3]=='E')&(received_data[4]=='A')&(received_data[5]=='D')&(received_data[6]=='Y')&(received_data[7]==' ')&
  (received_data[8]=='C')&(received_data[9]=='O')&(received_data[10]=='N')&(received_data[11]=='N')&(received_data[12]=='E')&(received_data[13]=='C')&(received_data[14]=='T'))){ //connected to server successful
  
  //********* for WiFi mode **********
  /*if(((received_data[0]=='C')&(received_data[1]=='O')&(received_data[2]=='N')&(received_data[3]=='N')&(received_data[4]=='E')&(received_data[5]=='C')&(received_data[6]=='T'))
  |((received_data[0]=='A')&(received_data[1]=='L')&(received_data[2]=='R')&(received_data[3]=='E')&(received_data[4]=='A')&(received_data[5]=='D')&(received_data[6]=='Y')&(received_data[7]==' ')&
  (received_data[8]=='C')&(received_data[9]=='O')&(received_data[10]=='N')&(received_data[11]=='N')&(received_data[12]=='E')&(received_data[13]=='C')&(received_data[14]=='T'))){*/ //connected to server successful
    
    connection_to_server = true;//USART_send_string("\nyes_1\n");
    
    if(connection_type == false)
      network_registration_flag = true;
    
    //IOT_STATUS_ON;
    initialize_reading_variables();
    
  }else if((received_data[0]=='C')&(received_data[1]=='O')&(received_data[2]=='N')&(received_data[3]=='N')&(received_data[4]=='E')&(received_data[5]=='C')&(received_data[6]=='T')&(received_data[7]==' ')&(received_data[8]=='F')&(received_data[9]=='A')&(received_data[10]=='I')&(received_data[11]=='L')){  //connected to server successful

    //IOT_STATUS_OFF;
    initialize_reading_variables();
    start_WAN_TCP_client();

  }else if((received_data[0]=='C')&(received_data[1]=='L')&(received_data[2]=='O')&(received_data[3]=='S')&(received_data[4]=='E')){  //TCP connection close

    abruptly_closed_connection = true;
    initialize_reading_variables();
    //IOT_STATUS_OFF;
    start_WAN_TCP_client();

  }else if((received_data[0]=='S')&(received_data[1]=='E')&(received_data[2]=='T')&(received_data[3]=='|')){  //Tag|data,       SET|string1#string2#string3#string4#string5#string6#string7#string8#
        
    if(strIndex == 5){
      digit1 = 0;
      section_count = 0;
    }else{
      
      if(received_data[strIndex]=='#'){ //data separator  (avoid using '|' symbol)
        section_count++;
        digit1 = 0;//digit2 = 0;
      }else{

        if(section_count==0){
          parsed_data_1[digit1] = received_data[strIndex];
        }else if(section_count==1){
          parsed_data_2[digit1] = received_data[strIndex];
        }else if(section_count == 2){
          parsed_data_3[digit1] = received_data[strIndex];
        }else if(section_count == 3){
          parsed_data_4[digit1] = received_data[strIndex];
        }else if(section_count == 4){
          parsed_data_5[digit1] = received_data[strIndex];
        }else if(section_count == 5){
          parsed_data_6[digit1] = received_data[strIndex];
        }else if(section_count == 6){
          parsed_data_7[digit1] = received_data[strIndex];
        }else if(section_count == 7){//USART_send_string("\nyes2:");//USART_send_data(received_data[strIndex]);//USART_send_string("\n9");
          
          //blink_network_led(5,200);
          initialize_reading_variables();
          server_response = true;
          
        }
        digit1++;
      }
      strIndex--;
    }
  }else if((received_data[0]=='C')&(received_data[1]=='T')&(received_data[2]=='R')&(received_data[3]=='L')&(received_data[4]=='|')&(strIndex==5)){  //Response: CTRL|1

    switch(received_data[5]){//switch(received_data[strIndex]){
          case 0:
            //action 1
            break;
          case 1:
            //action 2
            break;
          case 2:
            //action 3
            break;
          case 3:
            //action 4
    }
    //blink_network_led(5,200);
    initialize_reading_variables();
    initiate_DATA_send_to_server = true;

  }else if(((received_data[0]=='R')&(received_data[1]=='Q')&(received_data[2]=='S')&(received_data[3]=='T')&(received_data[4]==' ')&(received_data[5]=='E')&(received_data[6]=='r')&(received_data[7]=='r')&(received_data[8]=='|'))
  |((received_data[5]=='O')&(received_data[6]=='K'))|((received_data[6]=='O')&(received_data[7]=='K'))
  |((received_data[0]=='/')&(received_data[1]=='C')&(received_data[2]=='o')&(received_data[3]=='n')&(received_data[4]=='n')&(received_data[5]=='e')&(received_data[6]=='c')&(received_data[7]=='t')&(received_data[8]=='e')&(received_data[9]=='d'))){  //
  //|(received_data[0]=='>')

    initialize_reading_variables();
    server_response = true;
        }else if((received_data[0]=='#')&(received_data[1]=='S')&(received_data[2]=='C')&(received_data[3]=='L')&(received_data[4]=='K')&(received_data[5]==':')){  //set clock #SCLK:yy/MM/dd,hh:mm:ss+zz~
      //set clock #SCLK:yy/MM/dd,hh:mm:ss+zz~

    if(received_data[strIndex] == '~'){
      server_response = true;
        
      //year
      time_characters[11] = received_data[6];
      time_characters[10] = received_data[7];

      //month
      time_characters[9] = received_data[9];
      time_characters[8] = received_data[10];

      //day
      time_characters[7] = received_data[12];
      time_characters[6] = received_data[13];

      //hour
      time_characters[5] = received_data[15];
      time_characters[4] = received_data[16];

      //minute
      time_characters[3] = received_data[18];
      time_characters[2] = received_data[19];

      //second
      time_characters[1] = received_data[21];
      time_characters[0] = received_data[22];

      present_year = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[11],time_characters[10]);
      present_month = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[9],time_characters[8]);
      present_day = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[7],time_characters[6]);

      present_hour = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[5],time_characters[4]);
      //present_minute = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[3],time_characters[2]);
      //present_second = getIntegerOfString('0','0','0','0','0','0','0','0',time_characters[1],time_characters[0]);

      //****** time zone computation ********
      signed char time_zone_holder = present_hour;
      time_zone_holder += time_zone;

      if(time_zone_holder < 0)
        time_zone_holder *= (-1);

      if(time_zone_holder > 23)
        time_zone_holder %= 24;

      present_hour = time_zone_holder;
      
      get_strings_of_value(present_hour, true, 2);
      time_characters[5] = string_digit_of_value[0];
      time_characters[4] = string_digit_of_value[1];
      //*************************************

      initialize_reading_variables();
    }
  }
}

void initialize_thingsweb_spm(void){
    
  //IOT_STATUS_ON;
  
  //USART_send_string("ATE0\r\n");
  
  start_WAN_TCP_client();
  wait_for_server_connection();
  disconnect_device_at_server();
  register_device_at_server();
  requestDateAndTime();
  request_signal_strength();
  for(uint8_t n=0;n<6;n++){
    request_user_data_command(n);
    _delay_ms(1000);
    
    request_settings_command(n);
    _delay_ms(1000);
  }

  _delay_ms(1000);
}

void loop_thingsweb_spm(void){

  if(initiate_DATA_send_to_server == true){//USART_send_string("\r\nYes_7p");//USART_send_string("\r\nYes_7f"); if((online_or_offline == true) | (activation_key_status == false)){
    requestDateAndTime();
    request_user_data_command();
    send_data_to_server();
  }
}

ISR(TIMER0_OVF_vect) {
  
  if(CHECKBIT(UCSR0A,RXC0)){
    received_data[strIndex] = UDR0;
    if((received_data[strIndex] >= 0x20) & (received_data[strIndex] <= 0x7E)){
      receiving_from_server = true;
      thingsweb_rxd_action();
      strIndex++;
    }else if((received_data[strIndex] == '\n') | (received_data[strIndex] == '\r')){
      initialize_reading_variables();
    }
  }
}

int main(void) {
  // put your setup code here, to run once:
  DDRD = 0xFE;
  //DDRC = 0x80;
  //DDRB = 0xFF;

  //IOT_STATUS_ON;
  
  setup_SFR();    //USART_send_string("System running\r\n");
  
  TCCR0B |= (1<<CS00);
  initialize_thingsweb_spm();

  while(1){// put your main code here, to run repeatedly:
    
    loop_thingsweb_spm();
  }
}
