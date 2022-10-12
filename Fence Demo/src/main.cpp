#include <Arduino.h>
#include <CayenneLPP.h>
#include <iostream>


        // Device Info

#define DEVEUI "D896E0FF00000468"                                //set 
#define APPEUI "70B3D57ED0041DA0"
#define APPKEY "15993DDDAFFA0D7D32D515A419743EE6"                                
  
        //DEBUG
#define DEBUG true

        //Constants

int SideSwitchPin_Right=7;
int SideSwitchPin_Left=6;

const long All_clear_interval= 30*(1000);         // in seconds
const long Alert_interval = 5*(1000);                // seconds
const int Breach_duration=3*(1000);     //in seconds

unsigned long startMillis_Right;
unsigned long startMillis_Left;
unsigned long previousMillis;

int SideSwitchVal_Right;
int SideSwitchVal_Left;

int RightAlarm_currentState;
int RightAlarm_lastState = LOW;

int LeftAlarm_currentState;
int LeftAlarm_lastState=LOW;

int BoxAlarm_currentState=LOW;

int Alert_sent=LOW;
int Status_code;
int Trigger;
int Connection_status;
String Connection_check;
String Send_check;

CayenneLPP lpp(51);  //payloadd size

           //Declare Function

String sendData(String command, const int timeout, boolean debug);
void SendPayload(int msg);
void UpdateState(String Side);
void Reconnect();

                //test code!!!!!!!!!!!!!!!!!!!

int CentreSwitchVal_Right=LOW;
int CentreSwitchVal_Left=LOW;

void setup() {

    Serial1.begin(115200);
    if (DEBUG){
    SerialUSB.begin(115200);
    while (!Serial1) {; }
    while (!SerialUSB) {; }
    }
    SerialUSB.println("Initialising System..........");    

                            // pinMode

    pinMode (SideSwitchPin_Right,INPUT_PULLDOWN);
    pinMode (SideSwitchPin_Left,INPUT_PULLDOWN);

                         //Setup up connection

                 //DEV INFO*
    sendData("AT+CDEVEUI=" + String(DEVEUI), 200, false);     //set DEVEUI
    sendData("AT+CAPPEUI=" + String(APPEUI), 200, false);    //set APPEUI
    sendData("AT+CAPPKEY=" + String(APPKEY), 200, false);   //set APPKEY

    SerialUSB.println("Connecting to Gateway......");
    Connection_check = sendData("AT+CJOIN=1,0,10,2", 30000, false);           // join lorawan         every 10 seconds,3 total attempts

    if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
        SerialUSB.println("Connection to Gateway Successful");
        Connection_status=1;

    }else{
        SerialUSB.println("Connection to Gateway Failed");
        Connection_status=0;}

    SerialUSB.println("Monitoring System Active");               //start fence monitoring regardless of connection status
  }

void loop() {
  
       // Reading sensor output

  SideSwitchVal_Right =!digitalRead(SideSwitchPin_Right);
  SideSwitchVal_Left =!digitalRead(SideSwitchPin_Left);

if (SideSwitchVal_Right==0 || CentreSwitchVal_Right==1)             //Right side Alarm state
    {
    RightAlarm_currentState= HIGH;    
    }else{
      RightAlarm_currentState=LOW;   
    }
 
if (SideSwitchVal_Left==0 || CentreSwitchVal_Left==1)              //Left side Alarm state
    {
    LeftAlarm_currentState=HIGH;
}else{
     LeftAlarm_currentState=LOW;
    }

                 //overall Alarm State
if (RightAlarm_currentState !=RightAlarm_lastState)
{                                       
  UpdateState("Right");} 
  
  RightAlarm_lastState = RightAlarm_currentState;      //change confusion

if (LeftAlarm_currentState !=LeftAlarm_lastState){
  UpdateState("Left");}
  
  LeftAlarm_lastState = LeftAlarm_currentState;


         //Alarm Logic      
             
if (LeftAlarm_currentState == HIGH                                                     //sends alert after timer
    && Alert_sent == LOW 
    && (millis()-startMillis_Left>Breach_duration ))
    {
      Trigger=13200;
      UpdateState("Alert");}
    
if (RightAlarm_currentState == HIGH 
    && Alert_sent == LOW 
    && (millis()-startMillis_Right>Breach_duration ))
    
    { Trigger=13300;
      UpdateState("Alert");
      }

if (BoxAlarm_currentState == HIGH 
    && Alert_sent == LOW )
    
    { Trigger=13400;
      UpdateState("Alert");
      }

                       //sending Alert lora payload
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
        SendPayload(Status_code);

        current_val=0;

        previousMillis=millis();
        }

if (BoxAlarm_currentState==LOW && LeftAlarm_currentState == LOW && RightAlarm_currentState == LOW &&  Alert_sent == HIGH)
{
  UpdateState("Alert");
  SerialUSB.println("Sending all clear signal....");
  SendPayload(13500);
                 
  }

}

void UpdateState(String Side){
 
  if (Side=="Right"){
    if (RightAlarm_currentState==HIGH)
            {startMillis_Right=millis();
            }  //check right side alarm state 
  }
   
  if (Side=="Left"){
    if (LeftAlarm_currentState==HIGH)
            {startMillis_Left=millis();
            }  //check left side alarm state
 }

  if (Side=="Alert")  //check right side alarm state
            {Alert_sent=!Alert_sent;
            if(Alert_sent==LOW)
            {
              previousMillis=millis();
              
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

void SendPayload(int msg){

    if (DEBUG) {
        SerialUSB.print("Status_code: ");
        SerialUSB.println(msg);


    if (Connection_status==0){                                                           //check if connected to gateway
      SerialUSB.println("Device NOT Connected to Gateway, Reconnecting....... ");
      Reconnect();                                                                           //try to reconnect
      }

    }
    lpp.reset();
    lpp.addLuminosity(1,msg);
    int size = lpp.getSize();
    char payload[size];

    for (int i = 0; i < size; i++) {
        payload[i] = *(lpp.getBuffer()+i);
    }
    char HEXpayload[51] = "";

    sprintf(HEXpayload,"AT+DTRX=1,2,%d,%02x%02x%02x%02x",size, payload[0], payload[1], payload[2], payload[3]);
    SerialUSB.println("Sending........");    
    
    Send_check = sendData((String)HEXpayload, 1500, DEBUG);           

    if(Send_check.indexOf("OK+SEND:04") > 0){                          // check if connected to Gateway     //FIX using RECV
        SerialUSB.println("SEND OK");
        Connection_status=1;

    }else{
        SerialUSB.println("SEND FAILED, Attempting to send Again");
        Connection_status=0;
        SendPayload(msg);
        }

}

void Reconnect(){

      Connection_check = sendData("AT+CJOIN=1,0,10,1", 12000, false);          //try to reconnect      every 10seconds,1 attempt

    if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
        SerialUSB.println("Reconnection to Gateway Successful");
        Connection_status=1;

    }else{
        SerialUSB.println("Reconnection to Gateway Failed");
        Connection_status=0;}
};