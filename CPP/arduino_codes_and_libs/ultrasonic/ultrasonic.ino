#include <HCSR04.h>

HCSR04 hc(1, 0); //initialisation class HCSR04 (trig pin , echo pin)
int th;

void setup()
{ 
  Serial.begin(9600);
  pinMode(2,OUTPUT);
}

void loop()
{
    th = hc.dist();
    Serial.println( th ); //return current distance (cm) in serial
    delay(60);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    if(th > 10 && th < 20)
    {
      digitalWrite(2,HIGH);
      Serial.println("high");
    }
    else
    {
      digitalWrite(2,LOW);
      Serial.println("LOW");
    }
}
