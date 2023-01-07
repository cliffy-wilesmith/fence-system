#include <Arduino.h>

int SideSwitchPin=9;
int CentreSwitchPin=8;


int RED_Pin=7;
int GREEN_Pin=6;

int CentreSwitchVal;
int SideSwitchVal;

void setup() {

  // Serial.begin(9600);

  pinMode (SideSwitchPin,INPUT_PULLUP);
  pinMode (CentreSwitchPin,INPUT_PULLUP);
    
  pinMode(RED_Pin, OUTPUT); 
  pinMode(GREEN_Pin, OUTPUT);
   
}

void loop() {
  
  SideSwitchVal =digitalRead(SideSwitchPin);           
  CentreSwitchVal =digitalRead(CentreSwitchPin);

  // Serial.print("Centre--------------------------");
  // Serial.println(CentreSwitchVal);
  // Serial.print("Side");
  // Serial.println(SideSwitchVal);

  if (SideSwitchVal==1 && CentreSwitchVal==0)           
  {
    digitalWrite(RED_Pin, LOW);
    digitalWrite(GREEN_Pin, HIGH);
    
  }else{
    digitalWrite(RED_Pin, HIGH);
    digitalWrite(GREEN_Pin, LOW); 
    
  } 

}