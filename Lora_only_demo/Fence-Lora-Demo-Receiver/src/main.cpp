
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
int packetnum ;

int a;
int b;
int c;

int Alert_code_array[ 9 ][ 3 ] ={ { 0, 0, 0 }, { 1, 0, 0 },{ 0, 1, 0 }, { 1, 1, 0 },{ 0, 0, 1 },{ 0, 0, 1 }, { 1, 0, 1 },{ 0, 1, 1 }, { 1, 1, 1 } };
 
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
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
  




  // digitalWrite(Left_RED_Pin, HIGH);
  // digitalWrite(Left_GREEN_Pin, HIGH);
  
  // delay(2000);

  // digitalWrite(Left_RED_Pin, LOW);
  // digitalWrite(Left_GREEN_Pin, LOW);
  
  // digitalWrite(Right_RED_Pin, HIGH);
  // digitalWrite(Right_GREEN_Pin, HIGH);
  
  // delay(2000);

  // digitalWrite(Right_RED_Pin, LOW);
  // digitalWrite(Right_GREEN_Pin, LOW);

  // digitalWrite(Alert_YELLOW_Pin, HIGH);
  // digitalWrite(System_BLUE_Pin, HIGH);
  
  // delay(2000);

  // digitalWrite(Alert_YELLOW_Pin, LOW);
  // digitalWrite(System_BLUE_Pin, LOW);
  
  
 
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
      // Serial.print("Got: ");

    
    // char HEXpayload[len];
    String HEXpayload=((char*)buf);
    // Serial.println(HEXpayload);

    String status_code = HEXpayload.substring(0,5);
    String LoraID = HEXpayload.substring(5,7);
    String packetnum = HEXpayload.substring(7,10);

    String lights = status_code.substring(4,5);
    Serial.println(lights.toInt());
    

    for (int i = 0; i <9; i++){
      if (lights.toInt() ==i){

        a = Alert_code_array[i][0];
        b = Alert_code_array[i][1];
        c = Alert_code_array[i][2];
      }
    

  
    }
    digitalWrite(Left_GREEN_Pin, !a);
    digitalWrite(Left_RED_Pin, a);


  
   digitalWrite(Right_RED_Pin, b);
    digitalWrite(Right_GREEN_Pin, !b);


    digitalWrite(System_BLUE_Pin, !c);

  
  // delay(2000);

  // digitalWrite(Right_RED_Pin, LOW);
  // digitalWrite(Right_GREEN_Pin, LOW);

  // digitalWrite(Alert_YELLOW_Pin, HIGH);
  // digitalWrite(System_BLUE_Pin, HIGH);
  
  // delay(2000);

  // digitalWrite(Alert_YELLOW_Pin, LOW);
  // digitalWrite(System_BLUE_Pin, LOW);


    
    // // status_code =payload_1
    // // LoraID=payload_2;
    // // packetnum=payload_3

    // Serial.println(payload_1);
    // Serial.println(payload_2);
    // Serial.println(payload_3);
    }
     
    else
    {
      Serial.println("Receive failed");
    }
  }

  // Serial.println(status_code);
  // Serial.println(LoraID);
  // Serial.println(status_code);

  // if(LoraID_check == LoraID){
  //   Serial.println("ID Check passed ");
  //   Serial.println(status_code);


    
    
  //   }
  

}