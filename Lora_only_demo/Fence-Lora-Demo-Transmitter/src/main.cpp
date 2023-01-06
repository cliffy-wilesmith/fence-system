//PIns 10,9,2 not to be used     //A4 - SDA       A5 - SCL

#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

#define DEBUG true

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 868.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int LoRaID=66;                                 //set  range:16-255 

int SideSwitchPin_Right=8;
int SideSwitchPin_Left=7;

int CentreSwitchPin_Right=6;
int CentreSwitchPin_Left=5;

const long Still_alive_interval= (2000);         // in seconds
            
const int Breach_duration=4*(1000);     //in seconds

unsigned long startMillis_Right;
unsigned long startMillis_Left;
unsigned long previousMillis;

int CentreSwitchVal_Right;
int CentreSwitchVal_Left;

int SideSwitchVal_Right;
int SideSwitchVal_Left;

int RightAlarm_currentState;
int RightAlarm_lastState = LOW;

int LeftAlarm_currentState;
int LeftAlarm_lastState=LOW;

int BoxAlarm_currentState=LOW;                       //Change w

int Alert_sent=LOW;
int Status_code;
int Trigger=13500;
int Connection_status;

void Send_LoraPayload(int msg, int alert);
void UpdateState(String Side);

void setup() 

{
  Serial.println("Initialising System..........\n");
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(9600);
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio Initialisation failed");
    while (1);
  }
  Serial.println("LoRa radio Initialisation  OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
                            // pinMode

    pinMode (SideSwitchPin_Right,INPUT_PULLUP);
    pinMode (SideSwitchPin_Left,INPUT_PULLUP);

    pinMode (CentreSwitchPin_Right,INPUT_PULLUP);
    pinMode (CentreSwitchPin_Left,INPUT_PULLUP);
   
                         //Setup up connection

    Send_LoraPayload(13500,0);         //setup connection

    Serial.println("Monitoring System Active"); 
    Serial.println();
}

void loop()
{
 // Reading sensor output

  SideSwitchVal_Right =digitalRead(SideSwitchPin_Right);           
  SideSwitchVal_Left =digitalRead(SideSwitchPin_Left); 

  CentreSwitchVal_Right =digitalRead(CentreSwitchPin_Right);  
  CentreSwitchVal_Left =digitalRead(CentreSwitchPin_Left);  

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

                 //overall Alarm State
if (RightAlarm_currentState !=RightAlarm_lastState)
{                                       
  UpdateState("Right");} 
  
  RightAlarm_lastState = RightAlarm_currentState;     

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


if (BoxAlarm_currentState==LOW && LeftAlarm_currentState == LOW && RightAlarm_currentState == LOW &&  Alert_sent == HIGH)     //Box
{ 
  UpdateState("Alert");
  Serial.println("ALERT LEVEL LOW");
                   
  }

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
        Send_LoraPayload(Status_code,Alert_sent);

        current_val=0;

        previousMillis=millis();
        }

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

  if (Side=="Alert")  
    {
      Alert_sent=!Alert_sent;
    }

//Sending update
  int current_val=0;                           // add current fence state info to update message
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
    Send_LoraPayload(Status_code,Alert_sent);

    current_val=0;
    previousMillis=millis();
    delay(20);
        }  
                          
void Send_LoraPayload(int msg, int alert){

  char HEXpayload[51] = "";
  sprintf(HEXpayload,"%i%i%i",msg,LoRaID,alert);

  Serial.println(HEXpayload);
      
  Serial.println("Sending...");

  rf95.send((uint8_t *)HEXpayload, 11);

  Serial.println("Waiting for packet to complete..."); delay(20);
  rf95.waitPacketSent();
  Serial.println("Packet Sent");
        }