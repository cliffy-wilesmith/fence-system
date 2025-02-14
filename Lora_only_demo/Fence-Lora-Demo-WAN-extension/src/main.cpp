#include <Arduino.h>
#include <Wire.h>
#include <CayenneLPP.h>

#define DEBUG true

     //test
#define DEVEUI "D896E0FF00000687"                                //set 
// #define APPEUI "70B3D57ED0041DA0"
// #define APPKEY "FF1FA6B66A5BB0E622A11C95BAE9C746"
#define APPEUI "70B3D57ED0041DA8"
#define APPKEY "FF1FA6B66A5BB0E622A11C95BAE9C747"   // URSALINK 

CayenneLPP lpp(51);  //payloadd size
String sendData(String command, unsigned long timeout, boolean debug);
void SendPayload(int msg);

    //Connection Variables

int Connection_status;
String Connection_check;
String Send_check;
String myString;

int nub=20000;

// void Reconnect();
void receiveEvent(int howMany);

void setup()
{
  Serial1.begin(115200);           //Start up LORAWAN module

  if (true){                                                    //DEBUG CODE
  SerialUSB.begin(115200);
  while (!Serial1) {;
  Serial.println("LoRa Module Initialisation failed"); }
  // while (!SerialUSB) {; }
  }

  SerialUSB.println("Initialising System..........\n");
  sendData("AT+CDEVEUI=" + String(DEVEUI), 200, false);     //set DEVEUI 
  sendData("AT+CAPPEUI=" + String(APPEUI), 200, false);    //set APPEUI
  sendData("AT+CAPPKEY=" + String(APPKEY), 200, false);   //set APPKEY

  // Connect to Gateway

  SerialUSB.println("Connecting to Gateway......");
  delay(5000);

  SerialUSB.println("STARTING............................");

  Connection_check = sendData("AT+CJOIN=1,0,6,0", 6100, false);           // join lorawan         6 seconds, 1 total attempt
  SerialUSB.println("Done......................................//////");
  // SerialUSB.println(Connection_check);

  if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
      SerialUSB.println("Connection to Gateway Successful");
      SerialUSB.println();
      Connection_status=1;

  }else{
      SerialUSB.println("Connection to Gateway Failed");
      SerialUSB.println();
      Connection_status=1;
      SerialUSB.println("Attempting Reconnection..............\n");
      Connection_check = sendData("AT+CJOIN=1,0,6,0", 6100, false);          //try to reconnect      6 seconds, 1 total attempt  

      if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
          SerialUSB.println("Reconnection to Gateway Successful");
          Connection_status=1;
      }else{
        SerialUSB.println("Reconnection to Gateway Failed \n");
        SerialUSB.println("Max attempts reached");
        Connection_status=1;}
  }

  SerialUSB.println("Monitoring System Active");               //start fence monitoring regardless of connection status
  SerialUSB.println();

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop()
{
  delay(20000);
  SendPayload(nub++);

}

void receiveEvent(int howMany)
{

  while(1 < Wire.available()) // loop through all but the last
  {
    
    byte Status_byteArray[5];
    Status_byteArray[0]=Wire.read();
    Status_byteArray[1]=Wire.read();
    Status_byteArray[2]=Wire.read();
    Status_byteArray[3]=Wire.read();
    Status_byteArray[4]=Wire.read();
         
  String myString = String((char *)Status_byteArray);

  // SerialUSB.println("worked"); 

  // SerialUSB.print(Status_byteArray[0]);
  // SerialUSB.print(Status_byteArray[1]);
  // SerialUSB.print(Status_byteArray[2]);
  // SerialUSB.print(Status_byteArray[3]);
  // SerialUSB.print(Status_byteArray[4]);

  // SerialUSB.println("  String 1:");         // print the integer
  // SerialUSB.println(myString);
  
  // int x = Wire.read();    // receive byte as an integer
 SerialUSB.println("  String 2:");
  SerialUSB.println(myString);
  
  if (myString!="13502" && myString!="13501" && myString!="13503" && myString!="NA" && myString!="SENT" ){

  int load=myString.toInt();
  SendPayload(load);
  String myString = "SENT";
  }
  }
}
       // Send Data Function

String sendData(String command, unsigned long timeout, boolean debug)
{ 
    String response=""; 
    Serial1.println(command);
    unsigned long time = millis();
    
    while ((time + timeout)>millis())
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
            // Send Payload Function

void SendPayload(int msg){

    if (DEBUG) {
        SerialUSB.print("ALERT, Sending Status_code: ");
        SerialUSB.println(msg);


    if (Connection_status==0){                                                           //check if connected to gateway
      SerialUSB.println("\n NOT CONNECTED to Gateway \n");
      // Reconnect();                                                                           //try to reconnect

       if (Connection_status==0){                     //Checking Reconnect
        SerialUSB.println("ALERT Can NOT be SENT");
        return;}

       if (Connection_status==1){
        SerialUSB.println("Attempting to send msg");
        }
      }
    }

    lpp.reset();                                           //Build Payload
    lpp.addLuminosity(1,msg);
    
    int size = lpp.getSize();
    char payload[size];

    for (int i = 0; i < size; i++) {
        payload[i] = *(lpp.getBuffer()+i);
    }
    char HEXpayload[51] = "";

    sprintf(HEXpayload,"AT+DTRX=1,2,%d,%02x%02x%02x%02x",size, payload[0], payload[1], payload[2], payload[3]);
      
    Send_check = sendData((String)HEXpayload, 3000, false);
     

    if(Send_check.indexOf("OK+SEND") > 0){
      SerialUSB.println("\nSEND check OK");

    }else{
        SerialUSB.println("SEND check FAILED");
        }        

    if(Send_check.indexOf("OK+RECV") > 0){                          // check if connected to Gateway   
        SerialUSB.println("\nRECV check OK");
        Connection_status=1;

    }else{
        SerialUSB.println("RECV check FAILED");
        Connection_status=1;
        SerialUSB.println("Attempting Resend [1 of 1]..............\n");
       
        Send_check = sendData((String)HEXpayload, 3000, false);

        if(Send_check.indexOf("OK+SEND") > 0){
        SerialUSB.println("\nSEND check OK");

        }else{
        SerialUSB.println("SEND check FAILED AGAIN");
              }        

        if(Send_check.indexOf("OK+RECV") > 0){                          // check if connected to Gateway   
        SerialUSB.println("\nRECV check OK, RESEND OK");
        Connection_status=1;

          }else{

            SerialUSB.println("\nRESEND FAILED Max attempts reached ");   
            }
        }

}

// void Reconnect(){

//   SerialUSB.println("Attempting Reconnection [1 of 3]..............\n");
//   Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);          //try to reconnect      every 10seconds,1 attempt 

//   if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
//     SerialUSB.println("Reconnection to Gateway Successful");
//     Connection_status=1;
//     return;

//   }else{
//     SerialUSB.println("Reconnection to Gateway Failed \n");
//     SerialUSB.println("Attempting Reconnection [2 of 3]..............\n");
//     Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);
    
      
//     if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
//       SerialUSB.println("Reconnection to Gateway Successful");
//       Connection_status=1;
//       return;

//     }else{
//       SerialUSB.println("Reconnection to Gateway Failed \n");
      
//       SerialUSB.println("Attempting Reconnection [3 of 3]..............\n");
//       Connection_check = sendData("AT+CJOIN=1,0,10,0", 5000, false);
      
//       if(Connection_check.indexOf("Joined") > 0){                          // check if connected to Gateway
//         SerialUSB.println("Reconnection to Gateway Successful");
//         Connection_status=1;
//         return;
//       }else{
//         SerialUSB.println("Reconnection to Gateway Failed");
//         SerialUSB.println();
//         SerialUSB.println("Max attempts reached");
//         Connection_status=0;}
//     }
//   }
// };