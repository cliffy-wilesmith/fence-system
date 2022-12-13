
//PIns 10,9,2 not to be used
#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>
#include <CayenneLPP.h>
#include <RH_RF95.h>
 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 868.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int LoraID_check=66;

int Left_RED_Pin=5;
int Left_GREEN_Pin=4;

int Right_RED_Pin=8;
int Right_GREEN_Pin=7;

int Alert_YELLOW_Pin = 6;
int System_BLUE_Pin = 3;

int status_code;
int LoraID ;

int Left_LED_Values;
int Right_LED_Values;
int Status_LED_Values;
int Alert_LED_Values;

int Alert_code_array[ 9 ][ 3 ] ={ { 0, 0, 0 }, { 1, 0, 0 },{ 0, 1, 0 }, { 1, 1, 0 },{ 0, 0, 1 },{ 0, 0, 1 }, { 1, 0, 1 },{ 0, 1, 1 }, { 1, 1, 1 } };   //array to hold LED values based on status code


const long Still_alive_interval= (60500);
unsigned long previousMillis=0;

void setup() 
{
  pinMode(Left_RED_Pin, OUTPUT); 
  pinMode(Left_GREEN_Pin, OUTPUT);

  pinMode(Right_RED_Pin, OUTPUT);
  pinMode(Right_GREEN_Pin, OUTPUT);
  
  pinMode(Alert_YELLOW_Pin, OUTPUT);
  pinMode(System_BLUE_Pin, OUTPUT);    
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  // while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio Initialisation failed");
    while (1);
  }
  Serial.println("LoRa radio Initialisation OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
}
 
void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
     
    RH_RF95::printBuffer("Received: ", buf, len);
   
    String HEXpayload=((char*)buf);
    // Serial.println(HEXpayload);

    String status_code = HEXpayload.substring(0,5);
    String LoraID = HEXpayload.substring(5,7);
    String lights = status_code.substring(4,5);
    String Alert_LED_Values = status_code.substring(1,2);

    Serial.println(Alert_LED_Values);
    
    
    
    // Serial.println(lights.toInt());
    
    if (LoraID_check==(LoraID.toInt()))
    {

      for (int i = 0; i <9; i++){
        if (lights.toInt() ==i){

          Left_LED_Values = Alert_code_array[i][0];
          Right_LED_Values = Alert_code_array[i][1];
          Status_LED_Values = Alert_code_array[i][2];
           }
      }
    
      digitalWrite(Left_GREEN_Pin, !Left_LED_Values);
      digitalWrite(Left_RED_Pin, Left_LED_Values);

      digitalWrite(Right_RED_Pin, Right_LED_Values);
      digitalWrite(Right_GREEN_Pin, !Right_LED_Values);

      digitalWrite(System_BLUE_Pin, !Status_LED_Values);

      if (6==(Alert_LED_Values.toInt())){

        digitalWrite(Alert_YELLOW_Pin, HIGH);
      
      }else{
        digitalWrite(Alert_YELLOW_Pin, LOW);
      }

      previousMillis=millis();

    }else {

    Serial.println("LoraID check failed");
    }
    
  }else{
    Serial.println("Receive failed");
    }
  }

  //Still alive logic

  if (((millis()-previousMillis)>Still_alive_interval) ){
    
    Serial.println("System offline");
    digitalWrite(System_BLUE_Pin, LOW);
    previousMillis=millis();
  }

}
