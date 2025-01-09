#include <HCSR04.h>              //Include ultrsonic Sensor library
HCSR04 hc(13,12); //initialisation class HCSR04 (trig pin , echo pin)
int th;

//temp
int lm35 = A2;
int temp;
int temp_val; 

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
const int PulseWire = A3;
int Threshold = 550;
int myBPM;
PulseSensorPlayground pulseSensor;

#include <SoftwareSerial.h>
#include <TinyGPS++.h>           // Include TinyGPS++ library
TinyGPSPlus gps;
SoftwareSerial SoftSerial(3,4);
float latit,longi;

SoftwareSerial sim(5, 6);

//IOT
#define RX 11
#define TX 10
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "IT3CSCPHSZFDF5HS";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);

//buzzer
int buzzer = 2;

//limitswitchs
#define limit_s1 A0  
#define limit_s2 A1
int lm_sw1,lm_sw2;
int pre_state=0;
int steps=0;

#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup() 
{
  Serial.begin(9600);
  pinMode(lm35, INPUT);
  pinMode(limit_s1,INPUT_PULLUP);
  pinMode(limit_s2,INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);
  sim.begin(9600);
  delay(1000);  
  SoftSerial.begin(9600);
  lcd.backlight();
  lcd.init(); 
  lcd.print("SMART SHOES");
  lcd.setCursor(0,1);
  lcd.print("FOR V.I");
  delay(1000);
  lcd.clear();
//  esp8266.begin(115200);
//  sendCommand("AT",5,"OK");
//  sendCommand("AT+CWMODE=1",5,"OK");
//  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  if (pulseSensor.begin()) 
    Serial.println("PulseSensor object created!");
}

void loop() 
{
  limitsw();
  check_ultrasense();
  temp1();
  heartrate();
  loc();
  //updatew();
}

void limitsw()
{
  lm_sw2 = analogRead(limit_s2);
  if(lm_sw2 < 500 && pre_state == 0)
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
    else if(lm_sw2 > 500)
    {
      pre_state = 0;
    }
}

void check_ultrasense()
{
    //ultra_sense
    th = hc.dist();
    Serial.print("Distance:");
    Serial.println(th); //return current distance (cm) in serial
    delay(90);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    if(th > 10 && th < 20)
    {
      digitalWrite(buzzer,HIGH);
      delay(2000);
      digitalWrite(buzzer,LOW);
    }
   
}

int temp1()
{
  temp=analogRead(lm35);
  temp_val = (temp * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10);                                                                      
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  Serial.print("Temperature:");
  lcd.setCursor(13, 0);
  lcd.print(temp_val);
  Serial.println(temp_val);
  delay(1000);
  lcd.clear();
  return temp_val;
}

void heartrate()
{
  myBPM = pulseSensor.getBeatsPerMinute();          
  if (pulseSensor.sawStartOfBeat()) 
  {                   
    Serial.println("♥  A HeartBeat Happened ! "); 
    Serial.print("BPM: ");
    Serial.println(myBPM);                        
    lcd.setCursor(0, 0);
    lcd.print("HEART RATE");
    lcd.setCursor(0, 1);
    lcd.print(myBPM);
    delay(3000);
    lcd.clear();
  }
           
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
}

 void RecieveMessage()
{
  sim.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }

 void loc()
 {
   lm_sw1=analogRead(limit_s1);
   if(lm_sw1 < 500)
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

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(temp_val )+"&"+ field1 +"="+String(myBPM);
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
