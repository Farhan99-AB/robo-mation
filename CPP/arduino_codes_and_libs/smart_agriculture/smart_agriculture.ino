#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,9);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//IOT
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "7LR4SJIK9F27F3HB";   // Write API KEY
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
int lm35 = A5;
int temp;

//rain
const int capteur_A = A2;
int val_analogique;

//moist sens
int sensorPin = A0; 
int moist_Value;  
int limit = 300;

 //ph sens
int ph_Value;
int ph_sense = A3;

//water sens
int water;
int water_sens = A4;

//motor relay
int motor_r = 11;

//fan 
int fan = 12;

void setup() 
{
  Serial.begin(9600);             // Serial port to computer
  
  pinMode(capteur_A, INPUT);      //rain sensor
  pinMode(ph_sense, INPUT);
  pinMode(water_sens, INPUT);
  pinMode(motor_r, OUTPUT);
  pinMode(fan, OUTPUT);
  
  lcd.begin(16, 2);
  lcd.print("Smart");
  lcd.setCursor(0,1);
  lcd.print("Agriculture");
  delay(2000);
  lcd.clear();

  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

}

void loop() 
{
  valSensor1 = check_water();
  valSensor2 = temp1();
  valSensor3 = rain();
  valSensor4 = moist();
  valSensor5 = ph();
  //EstablishTCPconnection();
  //UpdatetoTS();
    if((valSensor4 > 600))
    {
      Serial.println("MOTOR START");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: ONN");
      lcd.clear();
      digitalWrite(motor_r, HIGH);
      delay(1500);
    }
    else
    {
      Serial.println("MOTOR STOP");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: OFF");
      lcd.clear();
      digitalWrite(motor_r, LOW);
      delay(1500);
    } 
    delay(10);
    
    if((valSensor2 > 30))
    {
      Serial.println("FAN START");
      lcd.setCursor(0,1);
      lcd.print("FAN: ONN");
      lcd.clear();
      digitalWrite(fan, HIGH);
      delay(1500);
    }
    else
    {
      Serial.println("FAN STOP");
      lcd.setCursor(0,1);
      lcd.print("FAN: OFF");
      lcd.clear();
      digitalWrite(fan, LOW);
      delay(1500);
    }
  updatew();
}

int check_water()
{
     water = analogRead(water_sens);
     Serial.print("Water Level: ");
     Serial.println(water);
     lcd.setCursor(0, 0);
     lcd.print("Water Level: ");
     lcd.setCursor(13, 0);
     lcd.print(water);
     delay(2000);
     lcd.clear();
     return water;
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
