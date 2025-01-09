#include <SoftwareSerial.h>
#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
TinyGPSPlus gps;

SoftwareSerial SoftSerial(4,3);
float latit,longi;

SoftwareSerial sim(10, 11);
#define limit_s 8    

int _timeout;
int a=0;

void setup()
    {
     pinMode(limit_s,INPUT_PULLUP);
      Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
      delay(100);
      Serial.println("System Started...");
      sim.begin(9600);   // Setting the baud rate of GSM Module
      delay(1000);  
      SoftSerial.begin(9600);
      Serial.print("setup done");  
    }

void loop()

  {
    int limit_state=digitalRead(limit_s);
    //Serial.println(limit_state);  
  if(limit_state == 0)
  {
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
 }  
 if (sim.available()>0)
   Serial.write(sim.read());
}


 void SendMessage(float x,float y)
{
  String latit= String(x,6);
  String longi= String(y,6);
  String link= "https://www.google.com/maps/place/";
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  sim.println("AT+CMGS=\"+919611417678\""); // Replace x with mobile number
  delay(5000);
  String SMS = link+latit+","+longi;
  sim.println(SMS);// The SMS text you want to send
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(5000);
  Serial.println("Message sent");
  delay(5000);
  fun();
}

void fun()
{
  fun();
}

 void RecieveMessage()
{
  sim.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
