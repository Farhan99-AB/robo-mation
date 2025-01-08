#include <SoftwareSerial.h>
#define tx 2
#define rx 3
SoftwareSerial bt(rx,tx); //RX, TX
void setup() 
{
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(tx, OUTPUT);
  pinMode(rx, INPUT);
}
void loop() 
{
  bt.write(123);
}
