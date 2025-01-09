#include <Keypad.h>

// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//IOT
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "OnePlus 7";       // AP NAME
String PASS = "12345678911";    // AP PASSWORD
String API = "X8P9C5PHRAX6JC03";   // Write API KEY
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

SoftwareSerial esp8266(RX,TX);

//////////KEYPAD
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//ldr
int bulbpin = 11;//9
int ldrpin = A3;
int valuee = 0;

//temp
int lm35 = A0;
int temp;
int temp_val;

//smoke 
int smokeA0 = A1;
// Your threshold value
int sensorThres = 96;
int smokesense;

//current 
#define Current_sensor A2  //The sensor analog input pin
float i;

//water spray
int wtr_spry = 12;

//buzzer
//int buzz = 12;

//limit
//int lt_sw = A1;
//int th;

//fan
int fan = 13;

void setup() 
{
  pinMode(smokeA0, INPUT);
  pinMode(Current_sensor, INPUT);
  pinMode(ldrpin, INPUT_PULLUP);
//  pinMode(lt_sw, INPUT_PULLUP);
  //pinMode(buzz, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(wtr_spry, OUTPUT);
  pinMode(bulbpin, OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  Serial.println("START");
  lcd.backlight();
  lcd.init(); 
  lcd.print("INDUSTRY");
  lcd.setCursor(0,1);
  lcd.print("AUTOMATION");
  delay(2000);
  lcd.clear();

  Serial.println("STRATING ESP");
  esp8266.begin(115200);
  Serial.println("START 2");
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

  key_pad();
}
void loop() 
{
  ldr();
  temp1();
  smoke();
  current();
  updatew();
}

void key_pad()
{
  lcd.clear();
  Serial.println("Enter Your Password");
  lcd.print("Enter password: ");
  String password="12345";
  String input_password;
  char code[5];
  while(1)
  {
    char key = customKeypad.getKey();
    delay(200);
    if (key)
    {
      //Serial.println(key);
      if(key == '*') 
      {
        input_password = ""; // reset the input password
        lcd.clear();
        lcd.print("Enter password: ");
      } 
      else if(key == '#') 
      {
        if(input_password == password) 
        {
            Serial.println("password is correct");
            lcd.clear();
            lcd.print("Authorized :)");
            lcd.setCursor(0,1);
            lcd.print("User");
            delay(2000);
            lcd.clear();
            break;
        } 
        else 
        {
          lcd.clear();
          lcd.print("Wrong Password:(");
          Serial.println("Entered Password: ");
          Serial.println(input_password);
          Serial.println("password is incorrect, try again");
          delay(2000);
          lcd.clear();
          lcd.print("Enter Password: ");
        }
        input_password = ""; // reset the input password
      } 
      else 
      {
        input_password += key; // append new character to input password string
        Serial.println(input_password);
        lcd.clear();
        lcd.print("Enter password: ");
        lcd.setCursor(0,1);
        lcd.print(input_password);
      }
    }
  }
}

void ldr()
{
  valuee = analogRead(ldrpin);

  Serial.print("LDR");
  Serial.println(valuee);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LDR: ");
  lcd.setCursor(5, 0);
  lcd.print(valuee);
  if(valuee > 500)
  {
    digitalWrite(bulbpin, HIGH);
  }
  else
  {
    digitalWrite(bulbpin, LOW);
  }
  
  delay(1500);
}

int temp1()
{
  temp=analogRead(lm35);
  temp_val = (temp * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10);
  lcd.clear();                                                                      
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  Serial.print("Temperature:");
  lcd.setCursor(13, 0);
  lcd.print(temp_val);
  if(temp_val > 40)
  {
    digitalWrite(fan, HIGH);
    delay(2000);
    digitalWrite(fan, LOW);
  }
  Serial.println(temp_val);
  delay(1000);
}

void smoke() 
{
  smokesense = analogRead(smokeA0);

  Serial.print("Smoke: ");
  Serial.println(smokesense);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smoke: ");
  lcd.setCursor(7, 0);
  lcd.print(smokesense);
  // Checks if it has reached the threshold value
  if (smokesense > sensorThres)
  {
    digitalWrite(wtr_spry, HIGH);
    //digitalWrite(buzz, HIGH);
    delay(3000);
    digitalWrite(wtr_spry, LOW);
    //digitalWrite(buzz, LOW);
  }
  delay(1500);
}

void current() 
{
  i = analogRead(Current_sensor);
  Serial.print("Current:");
  Serial.println(i);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current: ");
  lcd.setCursor(9, 0);
  lcd.print(i);
  delay(1500);                     //Modifying or removing the delay will change the way the signal is shown 
                                  //set it until you get the correct sinewave shap
}

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(temp_val) +"&"+ field2 +"="+String(0) +"&"+ field3 +"="+String(smokesense) +"&"+ field4 +"="+String(i) +"&"+ field5 +"="+String(valuee) ;
  Serial.println(getData);
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
