int distance=0;
int fuel_level=0;

int sensorPin = A0; //moisture sensor
int sensorValue;  
int limit = 300; 

#include <SoftwareSerial.h>
SoftwareSerial ser(2, 3);              // RX, TX

#include <TinyGPS++.h>           // Include TinyGPS++ library
#include <SoftwareSerial.h>      // Include software serial library
TinyGPSPlus gps;

#include <SoftwareSerial.h>
SoftwareSerial sim(10, 11);
#define limit_s 8    

SoftwareSerial SoftSerial(4,3);
float latit,longi;

#include <LiquidCrystal.h>
LiquidCrystal lcd(42,44,46,48,50,52);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

// IOT 1
String apiKey = "IG1QN7SQ45TR7UB5";    // Edit this API key according to your Account
String Host_Name = "GP13";                                                                                  // Edit Host_Name
String Password = "ganeshpatil";                                                                            // Edit Password

String cmd;

#define lm35 12

#define alco 15
int alco_level=0;

#define limit 37
int limit_state=0;

void setup() 
{
  ser.begin(115200);
  ser.println("AT+RST");               // Resetting ESP8266
  
  char inv = '"';
  String cmd = "AT+CWJAP";

  cmd += "=";
  cmd += inv;
  cmd += Host_Name;
  cmd += inv;
  cmd += ",";
  cmd += inv;
  cmd += Password;
  cmd += inv;
  ser.println(cmd);                    // Connecting ESP8266 to your WiFi Router



  Serial.begin(9600);   // Initiate a serial communication
  sim.begin(9600); 
  SoftSerial.begin(9600);
  
  pinMode(limit,INPUT_PULLUP);
  pinMode(13, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("SMART HELMET");
  delay(2000);
  lcd.clear();

}
void loop() 
{
  //EstablishTCPconnection();
  //UpdatetoTS();
  //fuel();
  //alcohol();
  //helmet();
}


void helmet()
{
  lcd.clear();
  limit_state=digitalRead(limit);
  while(digitalRead(limit)==1)
  {
    lcd.print("WEAR HELMET!!!!!!");
    delay(500);
    lcd.clear();
    delay(500);
  }
  lcd.print("HELMET ON");
  delay(2000);
  lcd.clear();
}

void EstablishTCPconnection()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";          // Establishing TCP connection
  cmd += "184.106.153.149";                       // api.thingspeak.com
  cmd += "\",80";                                 // port 80

  ser.println(cmd);
  Serial.println(cmd);
  if (ser.find("Error")) 
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

void UpdatetoTS()
{
  String state1 = String(distance);               // Converting them to string as to send it through URL
  String state2 = String(limit_state);
  String state3 = String(alco_level);
  String state4 = String(fuel_level);


  String getStr = "GET /update?api_key=";         // prepare GET string
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(state1);
  getStr += "&field2=";
  getStr += String(state2);
  getStr += "&field3=";
  getStr += String(state3);

  getStr += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());                // Total Length of data

  ser.println(cmd);
  Serial.println(cmd);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connection");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("data sent");
  delay(1000);
  lcd.clear();

  delay(1000);

  if (ser.find(">")) 
  {
    ser.print(getStr);
    Serial.print(getStr);
  }
  else 
  {
    ser.println("AT+CIPCLOSE");                  // closing connection
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
  delay(1000);                                  // Update after every 15 seconds 
}

void temp1()
{
     int temp=analogRead(lm35)*0.488;                                                                         
     lcd.setCursor(1, 0);
     lcd.print("Temperature");
     lcd.setCursor(0, 1);
     lcd.print(temp);
     delay(1000);
     lcd.clear();
          
     if(temp>37)                                                                     
     {
        lcd.setCursor(1, 0);
        lcd.print("ENGINE HEATED");
        delay(2000);
        lcd.clear();
      }
      else
      {
        delay(3000);
        lcd.clear();   
      }
} 

void moist() 
{

 sensorValue = analogRead(sensorPin); 
 Serial.println("Analog Value : ");
 Serial.println(sensorValue);
 
 if (sensorValue<limit) {
 digitalWrite(13, HIGH); 
 }
 else {
 digitalWrite(13, LOW); 
 }
 
 delay(1000); 
}

void pos()
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
