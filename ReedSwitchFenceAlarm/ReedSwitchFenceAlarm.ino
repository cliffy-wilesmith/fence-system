int CentreSwitch_Pin_Right=7;
int CentreSwitch_Pin_Left=6;
int CentreSwitchVal_Right;
int CentreSwitchVal_Left;

int SideSwitchPin_Right=9;
int SideSwitchPin_Left=8;
int SideSwitchVal_Right;
int SideSwitchVal_Left;
int dt=250;
int Right_bluePin=2;               
int Left_greenPin=3;     
int AlarmPin=4;

int RightAlarm;
int LeftAlarm;


void setup() {
  // put your setup code here, to run once:
  
  pinMode(CentreSwitch_Pin_Right,INPUT_PULLDOWN);
  //digitalWrite(CentreSwitch_Pin_Right,LOW);
  pinMode (SideSwitchPin_Right,INPUT_PULLUP);
  //digitalWrite(SideSwitchPin_Right,HIGH);

  pinMode(CentreSwitch_Pin_Left,INPUT_PULLDOWN);
  //digitalWrite(CentreSwitch_Pin_Left,LOW);
  pinMode (SideSwitchPin_Left,INPUT_PULLUP);
  //digitalWrite(SideSwitchPin_Left,HIGH);

  SerialUSB.begin(9600);    // Initialize Native USB port
  pinMode(Left_greenPin,OUTPUT);
  pinMode(Right_bluePin,OUTPUT);
  pinMode(AlarmPin,OUTPUT);
  }

void loop() {
  // put your main code here, to run repeatedly:
   
  CentreSwitchVal_Right=digitalRead(CentreSwitch_Pin_Right);
  SideSwitchVal_Right=digitalRead(SideSwitchPin_Right);

  CentreSwitchVal_Left=digitalRead(CentreSwitch_Pin_Left);
  SideSwitchVal_Left=digitalRead(SideSwitchPin_Left);


if (SideSwitchVal_Right==0 
    || CentreSwitchVal_Right==1)
    {
    digitalWrite(Right_bluePin,LOW);
    RightAlarm= HIGH;
}
    else{
      digitalWrite(Right_bluePin,HIGH);
      RightAlarm=LOW;
    }
    
if (SideSwitchVal_Left==0 
    || CentreSwitchVal_Left==1)
    {
    
    digitalWrite(Left_greenPin,LOW);
    LeftAlarm=HIGH;
}
    else{
      digitalWrite(Left_greenPin,HIGH);
      LeftAlarm=LOW;
    }
if (LeftAlarm == HIGH || RightAlarm == HIGH){
  digitalWrite(AlarmPin,HIGH);
}
else{digitalWrite(AlarmPin,LOW);}




SerialUSB.print("  Centre L = ");
SerialUSB.print(digitalRead(CentreSwitch_Pin_Left));

SerialUSB.print("  Centre R = ");
SerialUSB.print(digitalRead(CentreSwitch_Pin_Right));

SerialUSB.print("  Side R = ");
SerialUSB.print(SideSwitchVal_Right,DEC);

SerialUSB.print("  Side L = ");
SerialUSB.println(SideSwitchVal_Left,DEC);
delay(dt);
}
