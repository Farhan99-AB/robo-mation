// Interfacing Arduino with NEO-6M GPS module

//GPS connections
//TX of module to pin 4;

//GSM connections
//TX of the module to pin 10;
//RX of the module to pin 11;

#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
TinyGPSPlus gps;
SoftwareSerial SoftSerial(5,6);    // Configure SoftSerial library
float latit,longi;

SoftwareSerial sim(10, 11);
int _timeout;
int a=0;
String _buffer;
//String number = "+919590172717";
String number = "+919611417678";

 
void setup(void) 
{ 
  pinMode(8,INPUT_PULLUP);
  delay(7000); //delay for 7 seconds to make sure the modules get the signal
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("System Started...");
  sim.begin(9600);
  delay(1000);
  
  SoftSerial.begin(9600);
  
  //SendMessage(9.999,3.333);
  Serial.print("setup done");  
}
 
void loop() 
{
 //a=digitalRead(8);
 //if(a==LOW)
 //{
  gpscheck:
  if (SoftSerial.available() > 0) 
  { 
    if (gps.encode(SoftSerial.read())) 
    { 
      if (gps.location.isValid()) 
      {
        latit=gps.location.lat();
        longi=gps.location.lng();
        Serial.println(latit, 6);
        Serial.println(longi, 6);
        SendMessage(latit,longi);
      }
      else
      {
        Serial.println("Location Invalid"); 
        goto gpscheck;
      }
    }
    else
    {
      goto gpscheck;
    }
  }
  else
    {
    goto gpscheck;
    }
 //}
}

float SendMessage(float x,float y)
{
  String latit= String(x,6);
  String longi= String(y,6);
  String link= "https://www.google.com/maps/place/";
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  
  //String SMS = latit+","+longi;
  String SMS = link+latit+","+longi;
  
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
  fun();
}

String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

void fun()
{
  fun();
}
