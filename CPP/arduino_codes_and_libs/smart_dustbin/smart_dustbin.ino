#include <Wire.h>  
#include <SoftwareSerial.h>

#include <Servo.h>
Servo myservo;

// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define pirPin 4
int calibrationTime = 30;
long unsigned int lowIn;
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int PIRValue = 0;

#include <HCSR04.h>
HCSR04 hc(8,9); //initialisation class HCSR04 (trig pin , echo pin)
int th;

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 2; //mcu > HX711 dout pin
const int HX711_sck = 3; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

int lc_weight; 

const int calVal_eepromAdress = 0;
unsigned long t = 0;

//IOT
#include <SoftwareSerial.h>
#define RX 6
#define TX 5
String AP = "FUTURE TECHNOLOGYS";       // AP NAME
String PASS = "9916143610";    // AP PASSWORD
String API = "J4EMB17KMGXYWHNQ";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);

int valSensor1,valSensor2;

void setup() 
{
  Serial.begin(57600); 
  pinMode(pirPin, INPUT);
  myservo.attach(10);
  lcd.backlight();
  lcd.init(); 
  lcd.print("Smart Dustbin");
  delay(1000);
  lcd.clear();
  Serial.println();
  Serial.println("Starting...");
  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
  #if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
  #endif
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) 
  {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else 
  {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  } 
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() 
{
  valSensor1 = check_ultrasense();
  valSensor2 = loadcell();
  lcd.print("weight: ");
  lcd.setCursor(8,0);
  lcd.print(valSensor2);
  delay(2000);
  lcd.clear();
  delay(500);
  PIRSensor();
  updatew();
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
    Serial.print("ultrasonic:");
    Serial.println( th ); //return current distance (cm) in serial
    delay(90);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    lcd.setCursor(0, 0);
    lcd.print("ultrasonic:");
    lcd.setCursor(11, 0);
    lcd.print(th);
    delay(1000);
    lcd.clear();
    return th;
}

int loadcell()
{
  int pv=0;
  Serial.println("Measuring Weight");
  lcd.print("Measuring Weight");
  delay(2000);
  lcd.clear();  
  while(1)
  {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 100; //increase value to slow down serial print activity
  
    // check for new data/start next conversion:
    if (LoadCell.update()) newDataReady = true;
  
    // get smoothed value from the dataset:
    if (newDataReady) 
    {
      if (millis() > t + serialPrintInterval) 
      {
        lc_weight = LoadCell.getData();
        Serial.print("Load_cell output val: ");
        Serial.println(lc_weight);
        newDataReady = 0;
        t = millis();
        if(pv == lc_weight && pv!=0)
        {
          return lc_weight;
        }
      }
      pv = lc_weight;
    }
  }
}

void PIRSensor() 
{
   long int p=millis();
   int s= 3000;
   while(millis() < p+s)
   {  
     if(digitalRead(pirPin) == HIGH) 
     {
        if(lockLow) 
        {
           PIRValue = 1;
           lockLow = false;
           Serial.println("Motion detected.");
           servo_open();
           delay(3000);
           servo_close();
        }
        takeLowTime = true;
     }
   }
}

void servo_open()
{
  Serial.println("Servo Open");
    myservo.write(180);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    lcd.print("Servo Open");
    delay(1000);
    lcd.clear();
    
}

void servo_close()
{
  Serial.println("Servo Close");
    myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
    lcd.print("Servo Close");
    delay(1000);
    lcd.clear();
}
