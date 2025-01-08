#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,9);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//IOT
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "IT3CSCPHSZFDF5HS";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);
#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11);

#include "Wire.h"  
#include "RTClib.h"

RTC_DS1307 RTC;

void setup ()

{
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  
  sim.begin(9600);
  
  Wire.begin();
  
  RTC.begin(); // load the time from your computer.
  
  if (! RTC.isrunning())
  
  {
  
  Serial.println("RTC is NOT running!");// This will reflect the time that your sketch was compiled
  
  RTC.adjust(DateTime(__DATE__, __TIME__));
  
  }
  Serial.print("setup done");
  SendMessage();

   lcd.begin(16, 2);  
   lcd.print("SMART ");
   lcd.setCursor(0,1);
   lcd.print("VENTILATOR");
   delay(2000);
   lcd.clear();
   
   esp8266.begin(115200);
   sendCommand("AT",5,"OK");
   sendCommand("AT+CWMODE=1",5,"OK");
   sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

}

void loop ()

{
  
  DateTime now = RTC.now();
  if(now.hour()==23 && now.minute()==40 && now.second()==10)
  //Change time of alarm as you want
  {
  digitalWrite(13, HIGH);
  }
  Serial.println();
  Serial.print("  Time: ");
  
  Serial.print(now.hour(), DEC);
  
  Serial.print(':');
  
  Serial.print(now.minute(), DEC);
  
  Serial.print(':');
  
  Serial.print(now.second(), DEC);
  
  Serial.println();
  Serial.print("-------------------------------------");
  Serial.println();
  
  delay(1000);

  if (sim.available()>0)
   Serial.write(sim.read());
  
  updatew();
}

void SendMessage()
{
  //String latit= String(x,6);
  //String longi= String(y,6);
  //String link= "https://www.google.com/maps/place/";
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  sim.println("AT+CMGS=\"+916363704393\""); // Replace x with mobile number
  delay(5000);
  String SMS = "1";
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

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(temp_val)+ "&"+ field2 +"="+String(bpm);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  lcd.print("Sending Data..");
  delay(1500);
  lcd.clear();
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
    lcd.print("Data Sent..");
    delay(1500);
    lcd.clear();
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
