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

  SerialUSB.begin(9600);

  // pinMode
  pinMode(CentreSwitch_Pin,   INPUT_PULLUP          );     //input mode
  pinMode(SideSwitch_Pin,    INPUT_PULLUP          );

  pinMode(GreenLED_Pin,OUTPUT);
  pinMode(Red_Pin,OUTPUT);

  }

void loop() {


   unsigned long currentMillis = millis();
  CentreSwitchVal=digitalRead(CentreSwitch_Pin);
  SideSwitchVal=digitalRead(SideSwitch_Pin);


  //Alarm logic

if (SideSwitchVal==1 && CentreSwitchVal==0)          //
    {
    digitalWrite(GreenLED_Pin,HIGH);
    digitalWrite(Red_Pin,LOW); 
    }else{
      digitalWrite(GreenLED_Pin,LOW);
      digitalWrite(Red_Pin,HIGH);

    }

    if (currentMillis - previousMillis >= interval) {
 
    previousMillis = currentMillis;
    SerialUSB.println("Centre Switch value");
    SerialUSB.println(CentreSwitchVal);
    SerialUSB.println("Side Switch value");
    SerialUSB.println(SideSwitchVal);
    
    
    }

    
 
}

         
