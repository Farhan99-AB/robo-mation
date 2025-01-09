#include <LiquidCrystal.h>
LiquidCrystal lcd(30,32,34,36,38,40);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//IOT
#include <SoftwareSerial.h>
#define RX 7
#define TX 8
String AP = "FUTURE TECHNOLOGYS";       // AP NAME
String PASS = "9916143610";    // AP PASSWORD
String API = "APK4MKVT9EVB678O";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false;

SoftwareSerial esp8266(RX,TX);

#include<SoftwareSerial.h>
#include <TinyGPS++.h>
static const int RXPin = 26, TXPin = 28;
static const uint32_t GPSBaud = 9600;

SoftwareSerial sim(10, 11);
TinyGPSPlus gps;

SoftwareSerial GPS(RXPin, TXPin);
float latit,longi;

#include <HCSR04.h>
HCSR04 hc(31,33); //initialisation class HCSR04 (trig pin , echo pin)
int th;

//RFID
#include <SPI.h>
#include <RFID.h>
#define SS_PIN 53 
#define RST_PIN 49
RFID rfid(SS_PIN, RST_PIN);

//temp
int lm35 = A0;
int temp;

//motor driver
const int enA = 9;
const int lf_motor = 3;
const int lb_motor = 4;

int valSensor1;
int valSensor2;

String rfidCard;

void setup() 
{
   Serial.begin(9600);
   // Set motor as OUTPUT
   pinMode(enA, OUTPUT);
   pinMode(lf_motor, OUTPUT);
   pinMode(lb_motor, OUTPUT); 
   sim.begin(9600);
   GPS.begin(GPSBaud);
   Serial.println("Starting the RFID Reader...");
   SPI.begin();
   rfid.init();
   lcd.begin(16, 2);
   lcd.print("Vehicle Speed");
   lcd.setCursor(0,1);
   lcd.print("Control");
   delay(2000);
   lcd.clear();
   
//   esp8266.begin(115200);
//   sendCommand("AT",5,"OK");
//   sendCommand("AT+CWMODE=1",5,"OK");
//   sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

}

void loop() 
{
  analogWrite(enA, 250);
  digitalWrite(lf_motor, HIGH);
  digitalWrite(lb_motor, LOW);
  valSensor1 = check_ultrasense();
  valSensor2 = temp1();
  spd_tag();
  //updatew();
  if (sim.available()>0)
    Serial.write(sim.read());
}  

//void loc()
//{
//  Serial.println("reached here1");
//  gpscheck:
//  Serial.println("reached here2");
//  if (GPS.available() > 0) 
//  { 
//    Serial.println("reached here3");
//    if (gps.encode(GPS.read())) 
//    { 
//      if (gps.location.isValid()) 
//      {
//        latit=gps.location.lat();
//        longi=gps.location.lng();
//        Serial.println(latit, 6);
//        Serial.println(longi, 6);
//        SendMessage(latit,longi);
//      }
//      else
//      {
//        Serial.println("Location Invalid"); 
//        goto gpscheck;
//      }
//    }
//    else
//    {
//      goto gpscheck;
//    }
//  }
//  else
//  {
//    goto gpscheck;
//  }
//}

void SendMessage()
{
  //String latit= String(x,6);
  //String longi= String(y,6);
  String link= "https://www.google.com/maps/place/";
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  sim.println("AT+CMGS=\"+918884496589\""); // Replace x with mobile number
  delay(5000);
  //String SMS = link+latit+","+longi;
  sim.println("KA29 EJ 1777");
  //sim.println(SMS);// The SMS text you want to send
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
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(valSensor1)+"&"+ field2 +"="+String(valSensor2);
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

int check_ultrasense()
{
    //ultra_sense
    th = hc.dist();
    Serial.print("Fuel Lvl:");
    Serial.println( th ); //return current distance (cm) in serial
    delay(90);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    lcd.setCursor(0, 0);
    lcd.print("Fuel Lvl: ");
    lcd.setCursor(10, 0);
    lcd.print(th);
    delay(1000);
    lcd.clear();
    return th;
}

int temp1()
{
  temp=analogRead(lm35);
  int temp_val = (temp * 4.88); /* Convert adc value to equivalent voltage */
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

void spd_tag()
{
  if (rfid.isCard()) 
  {
    Serial.println("In zone slowing speed");
    analogWrite(enA, 80);
    digitalWrite(lf_motor, HIGH);
    digitalWrite(lb_motor, LOW);
    lcd.print("Sending Location");
    //loc();
    SendMessage();
    lcd.clear();
    lcd.print("Location Sent");
    delay(1000);
    lcd.clear();
    rfid.halt();
  }
  else
  {
    Serial.println("Not in zone");
    analogWrite(enA, 200);
    digitalWrite(lf_motor, HIGH);
    digitalWrite(lb_motor, LOW);
  }
}
