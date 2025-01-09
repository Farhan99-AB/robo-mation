#include <SoftwareSerial.h>
#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
TinyGPSPlus gps;

#include <LiquidCrystal.h>
LiquidCrystal lcd(8,9,10,11,12,13);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);    // DEFINING ARDINO PINS

SoftwareSerial SoftSerial(6,7);   //DEFINING GPS PINS
float latit,longi;

SoftwareSerial sim(4, 3);   //DEFINING GSM PINS
#define limit_s A0    

int _timeout;
int a=0;
int limit_state;
int steps=0;
int buzzer = 5;
int pre_state=0;

void setup()
    {
      pinMode(limit_s,INPUT_PULLUP);    // SETTING LIMIT SWITCH AS INPUT
      Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
      delay(100);
      Serial.println("System Started...");
      sim.begin(9600);   // Setting the baud rate of GSM Module
      delay(1000);  
      SoftSerial.begin(9600);
      Serial.print("setup done");
      lcd.begin(16, 2);
      lcd.print("MINE DETECTOR");
      delay(2000);
      lcd.clear();  
    }

void loop()

  {
    limit_state=analogRead(limit_s);      // READING THE LIMIT SWITCH VALUES
    //Serial.println(limit_state);
    if(limit_state < 500 && pre_state == 0) //COUNTING THE NUMBER OF STEPS TAKEN
    {
      steps+=1;
      pre_state = 1;
      lcd.setCursor(1,0);
      lcd.print("Steps: ");
      Serial.print("Steps:");
      Serial.println(steps);
      lcd.setCursor(1,7);
      lcd.print(steps);   
    }
    else if(limit_state > 500)
    {
      pre_state = 0;
    }
  if(digitalRead(buzzer) == 1)    // IF METAL DETECTED SEND LOCATION
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
          SendMessage(latit,longi);     //SENDING LATTITUDE AND LONGITUDE
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


 void SendMessage(float x,float y)      // FUNCTION FOR GSM TO SEND MESSAGE 
{
  String latit= String(x,6);
  String longi= String(y,6);
  String link= "https://www.google.com/maps/place/";
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  sim.println("AT+CMGS=\"+919739264374\""); // Replace x with mobile number
  delay(5000);
  String SMS = link+latit+","+longi;
  sim.println(SMS);// The SMS text you want to send
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(5000);
  Serial.println("Message sent");
  delay(5000);
}

 void RecieveMessage()
{
  sim.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
