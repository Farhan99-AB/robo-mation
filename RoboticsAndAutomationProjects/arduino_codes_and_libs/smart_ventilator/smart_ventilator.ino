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
int temp_val;
int valSensor2;

SoftwareSerial esp8266(RX,TX);

int rel = 11;

//temp
int lm35 = A0;
int temp;

//SpO2
int spo = A1;
boolean countStatus;
int beat=0, bpm;
unsigned long millisBefore;

int buzz = 12;


void setup() 
{
  // put your setup code here, to run once:
   Serial.begin(9600);
   pinMode(buzz,OUTPUT);
   pinMode(rel, OUTPUT);
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

void loop() 
{  
    lcd.print("Detecting BPM!");
    delay(1500);
    lcd.clear();
    millisBefore = millis();
    while(1)
    {
      valSensor2 = analogRead(spo);
      if (countStatus == 0)
      {
        if (valSensor2 > 600)
        {
          countStatus = 1;
          beat++;
          Serial.println("Beat Detected!");
          Serial.print("Beat : ");
          Serial.println(beat);
        }
      }
      else
      {
        if (valSensor2 < 500)
        {
          countStatus = 0;
        }
      }
      if (millis() - millisBefore > 10000)
      {
        millisBefore = millis();
        break;
      }
      delay(1); 
  }
   bpm = beat * 6;
   lcd.clear();
   lcd.print("BPM: ");
   lcd.setCursor(5,0);
   lcd.print(bpm);
   delay(2000);
   lcd.clear();
   Serial.print("BPM : ");
   Serial.println(bpm);
   beat = 0;
   relay();
   temp1();
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
  delay(2000);
  lcd.clear();
  if(temp_val > 28)
  {
     Serial.println("BUZZ");
     lcd.setCursor(0,1);
     lcd.print("BUZZ");
     buzzer();
     delay(1000);
     lcd.clear();
  }
}

void relay()
{
    if(bpm < 72)
    {
      digitalWrite(rel,HIGH);
      Serial.println("Relay HIGH");
      lcd.print("Relay HIGH");
      delay(1500);
      lcd.clear();
    }
    else
    {
      digitalWrite(rel,LOW);
      Serial.println("Relay LOW");
      lcd.print("Relay LOW");
      delay(1500);
      lcd.clear();
    }
}

void buzzer()
{
  digitalWrite(buzz,HIGH);
  delay(2000);
  digitalWrite(buzz,LOW);
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
