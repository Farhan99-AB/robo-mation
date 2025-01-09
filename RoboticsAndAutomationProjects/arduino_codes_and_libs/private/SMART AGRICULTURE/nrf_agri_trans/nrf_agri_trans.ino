#include <SoftwareSerial.h>
#include <HCSR04.h>              //Include ultrsonic Sensor library

SoftwareSerial HC12(9, 10); // HC-12 TX Pin, HC-12 RX Pin
//ultrasense
HCSR04 hc(12, 13); //initialisation class HCSR04 (trig pin , echo pin)
int th;

#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,11);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//IOT
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    //"9916143610"; // AP PASSWORD
String API = "54WSJXP38VEMXMWU";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
String field5 = "field5";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor1;
int valSensor2;
int valSensor3;
int valSensor4;
int valSensor5;
SoftwareSerial esp8266(RX,TX);


//temp
int lm35 = A1;
int temp;
//rain
const int capteur_A = A2;
int val_analogique;
//moist sens
int sensorPin = A0; 
int moist_Value;  
int limit = 300; 
int ph_Value;
int ph_sense = A3;

void setup() 
{
  Serial.begin(9600);             // Serial port to computer
  
  HC12.begin(9600);               // Serial port to HC12
  Serial.println("Radio ONN");
  pinMode(capteur_A, INPUT);
  pinMode(ph_sense, INPUT);
  lcd.begin(16, 2);
  lcd.print("Smart Irrigation");
  delay(2000);
  lcd.clear();

  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

}

void loop() 
{
  valSensor1 = check_ultrasense();
  valSensor2 = temp1();
  valSensor3 = rain();
  valSensor4 = moist();
  valSensor5 = ph();
  //EstablishTCPconnection();
  //UpdatetoTS();
    if((valSensor4 > 600))
    {
      Serial.println("START");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: ONN");
      int text = 1;
      HC12.write(text);                  //Sending the message to receiver
      delay(1500);
    }
    else
    {
      Serial.println("STOP");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: OFF");
      int text = 0;
      HC12.write(text);
      delay(1500);
    }
    //radio.write(&button_state, sizeof(button_state));  //Sending the message to receiver 
    delay(10);
  //}
  updatew();
}

int check_ultrasense()
{
    //ultra_sense
    th = hc.dist();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ULTRA SONIC:");
    delay(1000);
    lcd.setCursor(12, 0);
    lcd.print(th);
    delay(1000);
    lcd.clear();
    Serial.print("ULTRA SONIC:");
    Serial.println( th ); //return current distance (cm) in serial
    delay(90);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    return th;
}

int temp1()
{
  temp=analogRead(lm35)*0.488;                                                                         
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  Serial.print("Temperature:");
  lcd.setCursor(13, 0);
  lcd.print(temp);
  Serial.println(temp);
  delay(1000);
  lcd.clear();
  return temp;
}

int rain()
{
  val_analogique=analogRead(capteur_A);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RAIN: ");
  delay(1000);
  lcd.setCursor(6, 0);
  lcd.print(val_analogique);
  delay(1000);
  lcd.clear(); 
  Serial.print("Rain : ");
  Serial.println(val_analogique); 
  delay(1000);
  return val_analogique;
}

int moist()
{
  moist_Value = analogRead(sensorPin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOISTURE: ");
  delay(1000);
  lcd.setCursor(10, 0);
  lcd.print(moist_Value);
  delay(1000);
  lcd.clear(); 
  Serial.print("Mosit Value : ");
  Serial.println(moist_Value);
  return moist_Value;
}

int ph()
{
  ph_Value = analogRead(ph_sense);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PH VALUE: ");
  delay(1000);
  lcd.setCursor(10, 0);
  lcd.print(ph_Value);
  delay(1000);
  lcd.clear(); 
  Serial.print("ph Value : ");
  Serial.println(ph_Value);
  return ph_Value;
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
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
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(valSensor1)+ "&"+ field2 +"="+String(valSensor2)+ "&"+ field3 +"="+String(valSensor3)+ "&"+ field4 +"="+String(valSensor4)+ "&"+ field5 +"="+String(valSensor5);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
}
