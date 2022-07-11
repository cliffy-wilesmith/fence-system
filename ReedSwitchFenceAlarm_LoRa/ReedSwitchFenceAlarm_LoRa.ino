#include <bluefairy.h>
bluefairy::Scheduler scheduler;
#define DEBUG true

// Device Info

#define DEVEUI "D896E0FF00000240"
#define APPEUI "70B3D57ED0041DA0"
#define APPKEY "DAC6118A3445572B02EFB73443AF2EB9"


// Assign Pins
int CentreSwitch_Pin_Right=7;
int CentreSwitch_Pin_Left=6;
int SideSwitchPin_Right=9;
int SideSwitchPin_Left=8;
int Right_bluePin=2;               
int Left_greenPin=3;     
int Alarm_yellowPin=4;
int Alert_redPin=5;


// Assign Constants 


long int runtime = 20000;
const long All_clear_interval= 30*(1000);   // in seconds
const long Sensor_interval = 1*(100);                //  in milliseconds 
const int dt=500;
const int Breach_duration=5*(1000);     //in seconds

// Assign Variables

int counter = 0;
unsigned long startMillis;
unsigned long startMillis_Right;
unsigned long startMillis_Left;
unsigned long currentMillis;
unsigned long elapsedMillis;
unsigned long previousMillis;
unsigned long left_Millis;
unsigned long right_Millis;
int CentreSwitchVal_Right;
int CentreSwitchVal_Left;
int SideSwitchVal_Right;
int SideSwitchVal_Left;
int RightAlarm_currentState;
int RightAlarm_lastState = LOW;
int LeftAlarm_currentState;
int LeftAlarm_lastState=LOW;
int Alert_sent=LOW;


bool ModuleState = false;

void setup() {
  Serial1.begin(115200);
  SerialUSB.begin(115200);

  // pinMode
  pinMode(CentreSwitch_Pin_Right,INPUT_PULLDOWN);
  pinMode (SideSwitchPin_Right,INPUT_PULLUP);
  pinMode(CentreSwitch_Pin_Left,INPUT_PULLDOWN);
  pinMode (SideSwitchPin_Left,INPUT_PULLUP);
  pinMode(Left_greenPin,OUTPUT);
  pinMode(Right_bluePin,OUTPUT);
  pinMode(Alarm_yellowPin,OUTPUT);
  pinMode(Alert_redPin,OUTPUT);
    
  //Setup up connection
    
  //sendData("AT+CDEVEUI=" + String(DEVEUI), 3000, DEBUG);     //set DEVEUI
  //sendData("AT+CAPPEUI=" + String(APPEUI), 3000, DEBUG);    //set APPEUI
  //sendData("AT+CAPPKEY=" + String(APPKEY), 3000, DEBUG);   //set APPKEY
  //sendData("AT+CJOINMODE=0", 3000, DEBUG);                //set join mod "OTAA"
  //sendData("AT+CJOIN=1,0,10,1", 30000, DEBUG);           //join lorawan
  }

void loop() {
  
       // Reading sensor output
  CentreSwitchVal_Right=digitalRead(CentreSwitch_Pin_Right);
  CentreSwitchVal_Left=digitalRead(CentreSwitch_Pin_Left);
  SideSwitchVal_Right=digitalRead(SideSwitchPin_Right);
  SideSwitchVal_Left=digitalRead(SideSwitchPin_Left);

if (SideSwitchVal_Right==0 || CentreSwitchVal_Right==1)             //Right side Alarm state
    {
    digitalWrite(Right_bluePin,LOW); //turn off allclear led for right side
    RightAlarm_currentState= HIGH;    
    }else{
      digitalWrite(Right_bluePin,HIGH); //turn on led
      RightAlarm_currentState=LOW;   
    }
 
if (SideSwitchVal_Left==0 || CentreSwitchVal_Left==1)              //Left side Alarm state
    {
    digitalWrite(Left_greenPin,LOW);  //turn off allclear led for left side
    LeftAlarm_currentState=HIGH;
}else{
      digitalWrite(Left_greenPin,HIGH);
     LeftAlarm_currentState=LOW;
    }
            
if (LeftAlarm_currentState == HIGH || RightAlarm_currentState == HIGH){                     // Alarm LEDs     
  digitalWrite(Alarm_yellowPin,HIGH);
}
else{digitalWrite(Alarm_yellowPin,LOW);}
digitalWrite(Alert_redPin,Alert_sent);
        

                 //overall Alarm State
if (RightAlarm_currentState !=RightAlarm_lastState)
{                                       
  UpdateState("Right");} 
  
  RightAlarm_lastState = RightAlarm_currentState;      //change confusion

if (LeftAlarm_currentState !=LeftAlarm_lastState){
  UpdateState("Left");}
  
  LeftAlarm_lastState = LeftAlarm_currentState;




         //Alarm Logic      
             
if (LeftAlarm_currentState == HIGH 
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

if (LeftAlarm_currentState == LOW && LeftAlarm_currentState == LOW &&  Alert_sent == HIGH)
{
  UpdateState("Alert");
  } 




        //int Left_lora = LeftAlarm;
        //int Right_lora= RightAlarm;

        //char msg[30] = "";
        //sprintf(msg, "AT+DTRX=1,2,2,%02x%02x", Left_lora, Right_lora);
        //SerialUSB.println(msg);
        //sendData((String)msg, 3000, DEBUG);}


 //unsigned long currentMillis = millis();

 

//while (Serial1.available() > 0)
  //  {
    //    SerialUSB.write(Serial1.read());
      //  yield();
    //}
    //while (SerialUSB.available() > 0)
    //{
      //  Serial1.write(SerialUSB.read());
       // yield();
//    
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

  if (Side=="Alert")
            {Alert_sent=!Alert_sent;}  //check right side alarm state
            
  
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
