// Library to use other pins as TX and RX pins for bluetooth module
#include <SoftwareSerial.h>

const byte rxpin = 2;
const byte txpin = 3;

char k = 'A';

//Connect the Bluetooth module
SoftwareSerial bluetooth(rxpin, txpin);

void setup()
{
  //Initialize the bluetooth with baud rate 38400 bps
  Serial.begin(9600);
  bluetooth.begin(38400);
  
}

void loop()
{
  //Check for new data
  while(bluetooth.available()){
    //Remember new data
    k = bluetooth.read();
    //Print the data for debugging purposes
    Serial.println(k);
  }
  
}
