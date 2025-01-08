#include <SoftwareSerial.h>
#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
#include <LiquidCrystal.h>    
TinyGPSPlus gps;

SoftwareSerial SoftSerial(4,3);
float latit,longi;

SoftwareSerial sim(10, 11);
#define limit_s1 A0    
#define limit_s2 A1

const int rs = 12, en = 13, d4 = 5, d5 = 6, d6 = 7, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int _timeout;
int a=0;
unsigned long stime = 0;
unsigned long currentMillis;
const unsigned long period = 5000;

void setup()
    {
      //pinMode(12,OUTPUT);
      lcd.begin(16, 2);
      lcd.print("Security System");
      pinMode(limit_s1,INPUT_PULLUP);
      pinMode(limit_s2,INPUT_PULLUP);
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
    int limit_state1=analogRead(limit_s1);
    //int tt =stime - startMillis;
    //Serial.println(tt);
    //Serial.println(limit_state);  
  if(limit_state1 < 500)
  {
    Serial.println("Button 1 pressed");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.println("Accident Alert");
    stime = millis();
    while(millis()- stime < period)
    {
      Serial.println("waiting for button 2");
      int limit_state2=analogRead(limit_s2);
      Serial.println(limit_state2);
      if(limit_state2 < 500)
      {
        Serial.println("Button 2 pressed");
        while(1){}
      }
    }
      lcd.clear();
      lcd.print("Requesting Help");
      Serial.println("Requesting Help");
      delay(1000);
      lcd.clear();
      gpscheck:
      //Serial.print(2);
      if (SoftSerial.available() > 0) 
      { 
        //Serial.println("1");
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
  lcd.setCursor(0, 0);
  lcd.print("Sending Message");
  delay(1000);
  lcd.clear();
  sim.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  sim.println("AT+CMGS=\"+918496839755\""); // Replace x with mobile number
  delay(5000);
  String SMS = link+latit+","+longi;
  sim.println(SMS);// The SMS text you want to send
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(5000);
  Serial.println("Message sent");
  lcd.setCursor(0, 0);
  lcd.print("Message sent");
  delay(5000);
  lcd.clear();
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
