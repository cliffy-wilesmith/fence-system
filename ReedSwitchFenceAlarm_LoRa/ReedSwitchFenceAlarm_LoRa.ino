// Assign Pins
int CentreSwitch_Pin=  3  ;
int SideSwitch_Pin= 4 ;
int GreenLED_Pin=5;     
int Red_Pin= 6;


// Assign Constants 
const long interval = 500;


// Assign Variables
unsigned long startMillis;
unsigned long currentMillis;
unsigned long previousMillis;

int CentreSwitchVal;
int SideSwitchVal;

bool ModuleState = false;

void setup() {

  Serial.begin(9600);

  // pinMode
  pinMode(CentreSwitch_Pin,   INPUT          );     //input mode
  pinMode(SideSwitch_Pin,    INPUT           );

  pinMode(GreenLED_Pin,OUTPUT);
  pinMode(Red_Pin,OUTPUT);

  }

void loop() {


   unsigned long currentMillis = millis();
  CentreSwitchVal=digitalRead(CentreSwitch_Pin);
  SideSwitchVal=digitalRead(SideSwitch_Pin);


  //Alarm logic

if (SideSwitchVal==0 || CentreSwitchVal==1)          //
    {
    digitalWrite(GreenLED_Pin,HIGH);
    digitalWrite(Red_Pin,LOW); 
    }else{
      digitalWrite(GreenLED_Pin,LOW);
      digitalWrite(Red_Pin,HIGH);

    }

    if (currentMillis - previousMillis >= interval) {
 
    previousMillis = currentMillis;
    Serial.println("Centre Switch value");
    Serial.println(CentreSwitchVal);
    Serial.println("Side Switch value");
    Serial.println(SideSwitchVal);
    
    
    }

    
 
}

                     
    
