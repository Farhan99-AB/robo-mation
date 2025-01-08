// Include Arduino Wire library for I2C
#include <Wire.h>  
#include <RTClib.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//IOT
#define RX 6
#define TX 7
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "CH3V4VL1U0JUO35N";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);

SoftwareSerial mySerial(2, 3); // (TX,RX)
RTC_DS1307 RTC;
Servo servo1;
Servo servo2;
Servo servo3;
int med = 0;

int val;
int pos;

int sw = A0;
int sw_state;

int buzz = 13;
void setup ()
{
  pinMode(buzz,OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);
  servo1.write(0);
  servo2.write(90);
  servo3.write(90);
   
  Serial.begin(9600);
  lcd.backlight();
  lcd.init(); 
  lcd.print("MEDICAL");
  lcd.setCursor(0,1);
  lcd.print("DISPENSOR");
  delay(2000);
  lcd.clear();
  mySerial.begin(9600);
  Wire.begin();
  RTC.begin(); // load the time from your computer.
  if (! RTC.isrunning())
  {
    Serial.println("RTC is NOT running!");// This will reflect the time that your sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop ()
{    
  lcd.clear();
  sw_state = analogRead(sw);
  DateTime now = RTC.now();
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.println(now.second(), DEC);
  lcd.print(now.hour(),DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.println(now.second(), DEC);
  if(now.hour()==19 && now.minute()==20 && now.second()==00)
  //Change time of alarm as you want
  {
    Serial.println("Servo 1");
    servo3.write(90);
    SendMessage();
    servo_1();
  }
  if(now.hour()==19 && now.minute()==22 && now.second()==00)
  //Change time of alarm as you want
  {
    Serial.println("Servo 2");
    servo1.write(0);
    SendMessage();
    servo_2();
  }

  if(now.hour()==19 && now.minute()==24 && now.second()==00)
  //Change time of alarm as you want
  {
    Serial.println("Servo 3");
    servo2.write(90);
    SendMessage();
    servo_3();
  }
  lt_switch();
  if (mySerial.available()>0)
   Serial.write(mySerial.read());
}

void servo_1()
{
    for (pos = 0; pos <= 90; pos += 1) 
    { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo1.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    buzzer();
    delay(2000);
}

void servo_2()
{
    for (pos = 90; pos >= 30; pos -= 1) 
    { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo2.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    buzzer();
    delay(2000);
}

void servo_3()
{
    for (pos = 90; pos >= 0; pos -= 1) 
    { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo3.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    buzzer();
    delay(2000);
}

void buzzer()
{
  lcd.clear();
  digitalWrite(buzz,HIGH);
  lcd.print("MEDICINE TIME");
  delay(2000);
  digitalWrite(buzz,LOW);
  lcd.clear();
}

void lt_switch()
{
  if(sw_state < 500)
  {
    med = 1;
    servo1.write(0);
    servo2.write(90);
    servo3.write(90);
    SendMessage();
    updatew();
  }
}

void SendMessage()
{
  if(sw_state < 500)
  {
    mySerial.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
    delay(5000);  // Delay of 1000 milli seconds or 1 second
    Serial.println("Gsm Into SMS mode");
    mySerial.println("AT+CMGS=\"+919611417678\""); // Replace x with mobile number
    delay(5000);
    mySerial.println("MEDICINE TAKEN");// The SMS text you want to send
    mySerial.println((char)26);// ASCII code of CTRL+Z
    delay(5000);
    Serial.println("Message sent");
    delay(5000);
  }
  else
  {
    mySerial.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
    delay(5000);  // Delay of 1000 milli seconds or 1 second
    Serial.println("Gsm Into SMS mode");
    mySerial.println("AT+CMGS=\"+919611417678\""); // Replace x with mobile number
    delay(5000);
    mySerial.println("MEDICINE TIME");// The SMS text you want to send
    mySerial.println((char)26);// ASCII code of CTRL+Z
    delay(5000);
    Serial.println("Message sent");
    delay(5000);
  }
}

void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(med);
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
