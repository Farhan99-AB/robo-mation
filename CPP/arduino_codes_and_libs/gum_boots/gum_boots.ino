#include <SoftwareSerial.h>
#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
#include <HCSR04.h>              //Include ultrsonic Sensor library
TinyGPSPlus gps;

SoftwareSerial SoftSerial(5,6);
float latit,longi;

HCSR04 hc(31,33); //initialisation class HCSR04 (trig pin , echo pin)
int th;

SoftwareSerial sim(10, 11);
#define limit_s 12    
const int flexpin = A0;
int _timeout;
int a=0;

void setup()
    {
      pinMode(12,OUTPUT);
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
    //check_ultrasense();
//    check_flexsense();
    int limit_state=digitalRead(limit_s);
    Serial.println(limit_state);  
  if(limit_state == 0)
  {
    gpscheck:
    Serial.println("Reached HERE1");
    if (SoftSerial.available() > 0) 
    { 
      Serial.println("Reached HERE2");
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
  sim.println("AT+CMGS=\"+916363704393\""); // Replace x with mobile number
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

void check_ultrasense()
{
    int flexValue;
    flexValue = analogRead(flexpin);
    Serial.print("F Sensor");
    Serial.println(flexValue);

    //ultra_sense
    th = hc.dist();
    Serial.print( th ); //return current distance (cm) in serial
    delay(90);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    if((th > 10 && th < 20) || flexValue>1001)
    {
      digitalWrite(12,HIGH);
    }
    else
    {
      digitalWrite(12,LOW);
    }
   
}
