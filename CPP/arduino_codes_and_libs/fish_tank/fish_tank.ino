//IOT
#include <SoftwareSerial.h>
#define RX 8
#define TX 9
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "2EOUMD1RP6Q9WF1L";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);

// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//temp
int lm35 = A2;
int temp;
int temp_val; 

//moist sens
int moistpin = A0; 
int moist_Value;  
int limit = 300;

//pH
int ph_Value;
int ph_sense = A3;

//RELAYS
int r1 = 7;
int r2 = 6;
int r3 = 5;
int r4 = 4;

void setup() 
{
  pinMode(r1,OUTPUT);
  pinMode(r2,OUTPUT);
  pinMode(r3,OUTPUT);
  pinMode(r4,OUTPUT);
  Serial.begin(9600);
  lcd.backlight();
  lcd.init(); 
  lcd.print("AQUATIC");
  lcd.setCursor(0,1);
  lcd.print("FARMING");
  delay(2000);
  lcd.clear();
  digitalWrite(r1, HIGH);
  delay(2000);
  digitalWrite(r1, LOW);
  delay(2000);
  digitalWrite(r2, HIGH);
  delay(2000);
  digitalWrite(r2, LOW);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() 
{
  temp1();
  moist();
  ph();
  updatew();
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
}

void moist()
{
  moist_Value = analogRead(moistpin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOISTURE: ");
  lcd.setCursor(10, 0);
  lcd.print(moist_Value);
  delay(1000);
  lcd.clear(); 
  Serial.print("Mosit Value : ");
  Serial.println(moist_Value);
}

int ph()
{
  ph_Value = analogRead(ph_sense);
  if(ph_Value > 5)
  {
    digitalWrite(r3, HIGH);
    delay(2000);
    digitalWrite(r3,LOW);
    delay(2000);
    digitalWrite(r4, HIGH);
    delay(2000);
    digitalWrite(r4,LOW);
  }
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
}

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(temp_val)+"&"+ field2 +"="+String(moist_Value)+"&"+ field3 +"="+String(ph_Value);
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
  lcd.clear();
  lcd.print("Sending Data..");
  delay(1500);
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
    lcd.clear();
    lcd.print("Data Sent..");
    delay(1500);
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
