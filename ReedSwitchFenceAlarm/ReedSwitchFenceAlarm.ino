int CentreSwitchPin=12;
int CentreSwitchVal;
int SideSwitchPin=11;
int SideSwitchVal;
int dt=250;
int bluePin=6;
int redPin=5;
int AlarmPin=4;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(CentreSwitchPin,INPUT);
  digitalWrite(CentreSwitchPin,HIGH);
  pinMode (SideSwitchPin,INPUT);
  digitalWrite(SideSwitchPin,HIGH);
  Serial.begin(9600);
  pinMode(redPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  pinMode(AlarmPin,OUTPUT);
  }

void loop() {
  // put your main code here, to run repeatedly:
   
  CentreSwitchVal=digitalRead(CentreSwitchPin);
  SideSwitchVal=digitalRead(SideSwitchPin);


if (CentreSwitchVal==1 && SideSwitchVal==1 
    || CentreSwitchVal==0 && SideSwitchVal==0
    ||CentreSwitchVal==1 && SideSwitchVal==0){
    digitalWrite(bluePin,LOW);
    digitalWrite(redPin,HIGH);
    digitalWrite(AlarmPin,HIGH);
}
    else{
      digitalWrite(redPin,LOW);
      digitalWrite(bluePin,HIGH);
      digitalWrite(AlarmPin,LOW);
    }


Serial.print("Centre Switch = ");
Serial.print(CentreSwitchVal);
Serial.print("   Side Switch = ");
Serial.println(SideSwitchVal);
delay(dt);
}
