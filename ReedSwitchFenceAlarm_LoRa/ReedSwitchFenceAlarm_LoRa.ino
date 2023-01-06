#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 6

void setup(){
  SerialUSB.begin(9600);
  SerialUSB.println("DHT TEST PROGRAM ");
  SerialUSB.print("LIBRARY VERSION: ");
  SerialUSB.println(DHT11LIB_VERSION);
  SerialUSB.println();
  SerialUSB.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop(){
  int chk;
  SerialUSB.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:
                SerialUSB.print("OK,\t");
                break;
    case DHTLIB_ERROR_CHECKSUM:
                SerialUSB.print("Checksum error,\t");
                break;
    case DHTLIB_ERROR_TIMEOUT:
                SerialUSB.print("Time out error,\t");
                break;
    default:
                SerialUSB.print("Unknown error,\t");
                break;
  }
 // DISPLAT DATA
  SerialUSB.print(DHT.humidity,1);
  SerialUSB.print(",\t");
  SerialUSB.println(DHT.temperature,1);

  delay(2000);
}
