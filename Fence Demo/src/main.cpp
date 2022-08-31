#include <Arduino.h>
#include <PinDefinitionsAndMore.h>       //Define macros for input and output pin etc.
#include <IRremote.hpp>
#include <CayenneLPP.h>


       // Device Info

#define DEVEUI "D896E0FF00000466"
#define APPEUI "70B3D57ED0041DA0"
#define APPKEY "DAC6118A3245672B02EFB73443AF5EB1"

    //DEBUG
#define DEBUG true
#define DECODE_NEC 


        //Constants

int IrPin=8;    //Ir input pin

int SideSwitchPin_Right=7;
int SideSwitchPin_Left=6;

int validation=0;
int input;             // the current reading from the input pin
int previous_command;   //meaningless 
int command ;   // the previous reading from the input pin


String symbol="";
String status="";
String msg="";

unsigned long lastDebounceTime = 0;  // the last time the input changed
unsigned long debounceDelay = 200;    // the debounce time; increase if outputting multiple times

long int runtime = 20000;
const long All_clear_interval= 30*(1000);   // in seconds
const long Alert_interval = 500;                //  in milliseconds 
const int dt=500;
const int Breach_duration=3*(1000);     //in seconds


int counter = 0;
unsigned long startMillis;
unsigned long startMillis_Right;
unsigned long startMillis_Left;
unsigned long currentMillis;
unsigned long elapsedMillis;
unsigned long previousMillis;
unsigned long previousMillis2;
unsigned long left_Millis;
unsigned long right_Millis;


int SideSwitchVal_Right;
int SideSwitchVal_Left;

int RightAlarm_currentState;
int RightAlarm_lastState = LOW;
int LeftAlarm_currentState;
int LeftAlarm_lastState=LOW;

int Alert_sent=LOW;
int Val;
int count=0;

//Declare Function

String sendData(String command, const int timeout, boolean debug);
void SendPayload(String msg, int DataRate);
void UpdateState(String Side);


                //test code

int CentreSwitchVal_Right=LOW;
int CentreSwitchVal_Left=LOW;


void setup() {


    Serial1.begin(115200);
    SerialUSB.begin(115200);
    while (!Serial1) {; }
    SerialUSB.println("System setup");    
                         //Setup up connection

                 //DEV INFO
    sendData("AT+CDEVEUI=" + String(DEVEUI), 200, false);     //set DEVEUI
    sendData("AT+CAPPEUI=" + String(APPEUI), 200, false);    //set APPEUI
    sendData("AT+CAPPKEY=" + String(APPKEY), 200, false);   //set APPKEY

    // SerialUSB.println("Connecting......");
    sendData("AT+CJOIN=1,0,10,12", 30000, DEBUG);           //join lorawan
    
                        // pinMode

    pinMode (SideSwitchPin_Right,INPUT_PULLDOWN);
    pinMode (SideSwitchPin_Left,INPUT_PULLDOWN);

  }

void loop() {



  

       // Reading sensor output

  SideSwitchVal_Right =!digitalRead(SideSwitchPin_Right);
  SideSwitchVal_Left =!digitalRead(SideSwitchPin_Left);

 

  if( millis()-previousMillis2 >Alert_interval){
        SerialUSB.println(SideSwitchVal_Right);
        SerialUSB.println(SideSwitchVal_Left);

        previousMillis2=millis();
        }
        

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
      UpdateState("Alert");}
    
if (RightAlarm_currentState == HIGH 
    && Alert_sent == LOW 
    && (millis()-startMillis_Right>Breach_duration ))
    
    {
      UpdateState("Alert");
      }

if (Alert_sent==HIGH && ((millis()-previousMillis)>Alert_interval) )   //sending lora payload

{ 
    
        SerialUSB.println("Alert Sent");

        SerialUSB.println(SideSwitchVal_Right);
        SerialUSB.println(SideSwitchVal_Left);
        previousMillis=millis();
        }

  

if (LeftAlarm_currentState == LOW && LeftAlarm_currentState == LOW &&  Alert_sent == HIGH)
{
  UpdateState("Alert");
  }

}


void UpdateState(String Side){
 
  if (Side=="Right"){
    if (RightAlarm_currentState==HIGH)
    
            {startMillis_Right=millis();}  //check right side alarm state 
  }
   

  if (Side=="Left"){
    if (LeftAlarm_currentState==HIGH)
            {startMillis_Left=millis();}  //check left side alarm state
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
  
    String response = "";
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