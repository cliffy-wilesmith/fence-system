        //Included Libraries  //DONT USE PIN 5 (reset pin)

#include <Arduino.h>
#include <CayenneLPP.h>
#include <iostream>

       //Temperature

#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 10
int chk;


        //DEBUG STATE

#define DEBUG true

        // Device Info

#define DEVEUI "D896E0FF00000468"                                //set 
#define APPEUI "70B3D57ED0041DA0"
#define APPKEY "15993DDDAFFA0D7D32D515A419743EE6" 

     //test
// #define DEVEUI "D896E0FF00000687"                                //set 
// #define APPEUI "70B3D57ED0041DA0"
// #define APPKEY "FF1FA6B66A5BB0E622A11C95BAE9C746" 

        //Input Pins

int SideSwitchPin_Right=9;
int SideSwitchPin_Left=8;

int CentreSwitchPin_Right=7;
int CentreSwitchPin_Left=6;

      //Time Constants

const long Still_alive_interval= 3600*(1000);    // in seconds *(milliseconds)       
const long Alert_interval = 2*(1000);           // in seconds *(milliseconds)
const long Display_interval = 1*(1000);        // in seconds *(milliseconds)
const int Breach_duration=3*(1000);           // in seconds *(milliseconds)

      //Time Variables

unsigned long startMillis_Right;
unsigned long startMillis_Left;
unsigned long previousMillis;

      //State Variables

int CentreSwitchVal_Right;
int CentreSwitchVal_Left;
int SideSwitchVal_Right;
int SideSwitchVal_Left;

int RightAlarm_currentState;
int LeftAlarm_currentState;

int RightAlarm_lastState = LOW;
int LeftAlarm_lastState=LOW;
int BoxAlarm_currentState=LOW;                       
int Alert_sent=LOW;

    //Connection Variables

int Connection_status;
String Connection_check;
String Send_check;

    //Payload Variables

CayenneLPP lpp(51);  //payloadd size
int Status_code;
int Trigger;

      //Declare Custom Functions

String sendData(String command, const int timeout, boolean debug);
void SendPayload(int msg);
void UpdateState(String Side);
void Reconnect();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

    Serial1.begin(115200);           //Start up LORAWAN module

    if (true){                                                    //DEBUG CODE
    SerialUSB.begin(115200);
    while (!Serial1) {;
    Serial.println("LoRa Module Initialisation failed"); }
    // while (!SerialUSB) {; }
    }

    SerialUSB.println("Initialising System..........\n");    

                   // Pin Modes

    pinMode (SideSwitchPin_Right,INPUT_PULLUP);
    pinMode (SideSwitchPin_Left,INPUT_PULLUP);

    pinMode (CentreSwitchPin_Right,INPUT_PULLUP);
    pinMode (CentreSwitchPin_Left,INPUT_PULLUP);

                 //Update Device Info

    sendData("AT+CDEVEUI=" + String(DEVEUI), 200, false);     //set DEVEUI 
    sendData("AT+CAPPEUI=" + String(APPEUI), 200, false);    //set APPEUI
    sendData("AT+CAPPKEY=" + String(APPKEY), 200, false);   //set APPKEY


                // Connect to Gateway

    SerialUSB.println("Connecting to Gateway......");
    Connection_check = sendData("AT+CJOIN=1,0,10,1", 10000, false);           // join lorawan         every 10 seconds, 2 total attempts

    delay(400);
    if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
        SerialUSB.println("Connection to Gateway Successful");
        SerialUSB.println();
        Connection_status=1;

    }else{
        SerialUSB.println("Connection to Gateway Failed");
        SerialUSB.println();
        Connection_status=0;}

    SerialUSB.println("Monitoring System Active");               //start fence monitoring regardless of connection status
    SerialUSB.println();
  }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
       // Reading Values from sensor

  SideSwitchVal_Right =digitalRead(SideSwitchPin_Right);           
  SideSwitchVal_Left =digitalRead(SideSwitchPin_Left); 
 
  CentreSwitchVal_Right =digitalRead(CentreSwitchPin_Right);  
  CentreSwitchVal_Left =digitalRead(CentreSwitchPin_Left); 

           //Sensor Logic         

if (SideSwitchVal_Right==1 && CentreSwitchVal_Right==0)             //Right side Alarm state
    {
    RightAlarm_currentState=LOW; 
    }else{
    RightAlarm_currentState= HIGH; 
    }

if (SideSwitchVal_Left==1 && CentreSwitchVal_Left==0)              //Left side Alarm state
    {
    LeftAlarm_currentState=LOW;
    }else{
    LeftAlarm_currentState=HIGH;
    }

             //Alarm State
if (RightAlarm_currentState !=RightAlarm_lastState)
{                                       
  UpdateState("Right");} 
  RightAlarm_lastState = RightAlarm_currentState;     

if (LeftAlarm_currentState !=LeftAlarm_lastState)
{
  UpdateState("Left");}
  LeftAlarm_lastState = LeftAlarm_currentState;

         //Send Alert Logic      
             
if (LeftAlarm_currentState == HIGH                                                     //sends alert after timer
    && Alert_sent == LOW 
    && (millis()-startMillis_Left>Breach_duration ))
    {
      Trigger=13200;                                  //What caused the alert
      UpdateState("Alert");}
    
if (RightAlarm_currentState == HIGH 
    && Alert_sent == LOW 
    && (millis()-startMillis_Right>Breach_duration ))
    
    { Trigger=13300;                                //What caused the alert
      UpdateState("Alert");
      }

if (BoxAlarm_currentState == HIGH 
    && Alert_sent == LOW )
    
    { Trigger=13400;                            //What caused the alert
      UpdateState("Alert");
      }

                       //Sending Alert Payload

if (Alert_sent==HIGH && ((millis()-previousMillis)>Alert_interval) ) 

{ 

  

  
       int current_val=0;                           // add current fence state info to alert message
       Status_code=Trigger;
        if(LeftAlarm_currentState==HIGH){
          current_val+=1;
        }
        if(RightAlarm_currentState==HIGH){
          current_val+=2;
        }
        if(BoxAlarm_currentState==HIGH){
          current_val+=5;
        }
        Status_code+=current_val;

        SendPayload(Status_code);    //Calling the sendpayload function
        current_val=0;
        previousMillis=millis();
        }

if (BoxAlarm_currentState==LOW && LeftAlarm_currentState == LOW && RightAlarm_currentState == LOW &&  Alert_sent == HIGH)     //Box
{ 
  UpdateState("Alert");
  SerialUSB.println("Alert Level LOW");
                   
  }
              //Send Still alive Payload

if ((millis()-previousMillis)>Still_alive_interval){
  { 
      Trigger=13500;
      int current_val=0;                           // add current fence state info to alert message
      Status_code=Trigger;

      if(LeftAlarm_currentState==HIGH){
        current_val+=1;
      }
      if(RightAlarm_currentState==HIGH){
        current_val+=2;
      }
      if(BoxAlarm_currentState==HIGH){
        current_val+=5;
      }
      Status_code+=current_val;
      SendPayload(Status_code);               //Calling the sendpayload function

      current_val=0;
      previousMillis=millis();
      }
}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                           //Declared Functions

          
          //Update State Function
void UpdateState(String Side){
 
  if (Side=="Right"){                                //check right side alarm state 
    if (RightAlarm_currentState==HIGH)
       {startMillis_Right=millis();
      }  
  }
   
  if (Side=="Left"){                                //check left side alarm state
    if (LeftAlarm_currentState==HIGH)
      {startMillis_Left=millis();
   }  
 }

  if (Side=="Alert"){                                //check right side alarm state
    Alert_sent=!Alert_sent;
    if(Alert_sent==LOW)
      {previousMillis=millis();       
      }
    }
}


         // Send Data Function

String sendData(String command, const int timeout, boolean debug)
{ 
    String response=""; 
    Serial1.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (Serial1.available())
        {
            char c = Serial1.read();
            response += c;
        }
    }
    if (debug) 
    {
        SerialUSB.print(response);
    }
    return response;
}


            // Send Payload Function

void SendPayload(int msg){

    if (DEBUG) {
        SerialUSB.print("ALERT, Sending Status_code: ");
        SerialUSB.println(msg);


    if (Connection_status==0){                                                           //check if connected to gateway
      SerialUSB.println("\n NOT CONNECTED to Gateway \n");
      Reconnect();                                                                           //try to reconnect

       if (Connection_status==0){                     //Checking Reconnect
        SerialUSB.println("ALERT Can NOT be SENT");
        return;}

       if (Connection_status==1){
        SerialUSB.println("Attempting to send msg");
        }
      }
    }

    lpp.reset();                                           //Build Payload

    //TEMP

    chk = DHT.read(DHT11_PIN);
    lpp.addTemperature(1,(DHT.temperature));
    lpp.addRelativeHumidity(1,(DHT.humidity));

    lpp.addLuminosity(1,msg);
    
    int size = lpp.getSize();
    char payload[size];

    for (int i = 0; i < size; i++) {
        payload[i] = *(lpp.getBuffer()+i);
    }
    char HEXpayload[51] = "";

    // SerialUSB.println("\nSIZE TEMPERATURE");

    // SerialUSB.println(DHT.temperature);
    // SerialUSB.println(DHT.humidity);

    // sprintf(HEXpayload,"AT+DTRX=1,2,%d,%02x%02x%02x%02x",size, payload[0], payload[1], payload[2], payload[3]);
    sprintf(HEXpayload,"AT+DTRX=1,2,%d,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",size, payload[0], payload[1], payload[2], payload[3], payload[4], payload[5], payload[6], payload[7], payload[8], payload[9], payload[10]);  
    
    Send_check = sendData((String)HEXpayload, 2000, false);
    delay(500);   

    if(Send_check.indexOf("OK+SEND") > 0){
      SerialUSB.println("\nSEND check OK");

    }else{
        SerialUSB.println("SEND check FAILED");
        }        

    if(Send_check.indexOf("OK+RECV") > 0){                          // check if connected to Gateway   
        SerialUSB.println("\nRECV check OK");
        Connection_status=1;

    }else{
        SerialUSB.println("RECV check FAILED");
        Connection_status=0;
        SerialUSB.println("Attempting Resend [1 of 1]..............\n");

        Reconnect();
       
        Send_check = sendData((String)HEXpayload, 2000, false);
        delay(500);

        if(Send_check.indexOf("OK+SEND") > 0){
        SerialUSB.println("\nSEND check OK");

        }else{
        SerialUSB.println("SEND check FAILED AGAIN");
              }        

        if(Send_check.indexOf("OK+RECV") > 0){                          // check if connected to Gateway   
        SerialUSB.println("\nRECV check OK, RESEND OK");
        Connection_status=1;

          }else{

            SerialUSB.println("\nRESEND FAILED Max attempts reached ");   
            }
        }

}

void Reconnect(){

  SerialUSB.println("Attempting Reconnection [1 of 3]..............\n");
  Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);          //try to reconnect      every 10seconds,1 attempt
  delay(1000); 

  if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
    SerialUSB.println("Reconnection to Gateway Successful");
    Connection_status=1;
    return;

  }else{
    SerialUSB.println("Reconnection to Gateway Failed \n");
    SerialUSB.println("Attempting Reconnection [2 of 3]..............\n");
    Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);
    delay(1000); 
      
    if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
      SerialUSB.println("Reconnection to Gateway Successful");
      Connection_status=1;
      return;

    }else{
      SerialUSB.println("Reconnection to Gateway Failed \n");
      
      SerialUSB.println("Attempting Reconnection [3 of 3]..............\n");
      Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);
      delay(1000); 
      
      if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
        SerialUSB.println("Reconnection to Gateway Successful");
        Connection_status=1;
        return;
      }else{

        SerialUSB.println("Reconnection to Gateway Failed");
        SerialUSB.println();
        SerialUSB.println("Max attempts reached");
        Connection_status=0;}
    }
  }
};
