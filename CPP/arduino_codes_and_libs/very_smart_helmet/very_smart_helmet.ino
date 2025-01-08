#include <Keypad.h>

// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//IOT
#include <SoftwareSerial.h>
#define RX 11
#define TX 12
String AP = "SATURDAY";       // AP NAME
String PASS = "SATURDAY";    // AP PASSWORD
String API = "19C7NHPH1K96LFTU";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial esp8266(RX,TX);

#define alco A1
int alco_level=0;

#define limit A0
int limit_state=0;

#define float_sens A2
int fuel_level=0;

#define main_relay 10

//////////KEYPAD
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  
  pinMode(limit,INPUT_PULLUP);
  pinMode(float_sens,INPUT_PULLUP);
  pinMode(main_relay,OUTPUT);

  lcd.backlight();
  lcd.init(); 
  lcd.print("SMART HELMET");
  delay(1000);
  lcd.clear();

  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

  key_pad();
}
void loop() 
{
  helmet();
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
            lcd.print("Authorized");
            lcd.setCursor(0,1);
            lcd.print("User");
            delay(2000);
            lcd.clear();
            break;
        } 
        else 
        {
          lcd.clear();
          lcd.print("Wrong Password!!");
          Serial.println("Entered Password: ");
          Serial.println(input_password);
          Serial.println("password is incorrect, try again");
          delay(2000);
          lcd.clear();
          break;
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

void fuel()
{
  lcd.clear();
  fuel_level=analogRead(float_sens);
  if(fuel_level < 20)
  {
    Serial.println("LOW FUEL!!!!!");
    lcd.print("LOW FUEL");
    delay(2000);
    while(1)
    {
    }
  }
  else
  {
    digitalWrite(main_relay,HIGH);
    lcd.print("IGNITION ON");
    Serial.println("IGNITION ON");
    delay(2000);
    lcd.clear();
    lcd.print("Drive Safely ;)");
    delay(5000);
  }
}

void alcohol()
{
  lcd.clear();
  Serial.print("Alcohol level: ");
  alco_level=analogRead(alco);
  Serial.println(alco_level);
  if(alco_level>200)
  {
    Serial.println("Alcohol level exceeded");
    lcd.print("Alcohol Level");
    lcd.setCursor(0,1);
    lcd.print("Exceeded!!!!!!");
    delay(2000);
    lcd.clear();
    lcd.print("Not Safe");
    lcd.setCursor(0,1);
    lcd.print("        To Drive");
    digitalWrite(main_relay,LOW);
    while(1)
    { 
    }
  }
  else
  {
    fuel();
  }
}

void helmet()
{
  lcd.clear();
  limit_state=analogRead(limit);
  while(analogRead(limit) > 500)
  {
    lcd.print("WEAR HELMET!!!!!!");
    Serial.println("WEAR HELMET!!!!!!");
    delay(500);
    lcd.clear();
    delay(500);
  }
  lcd.print("HELMET ON");
  Serial.println("HELMET ON");
  delay(2000);
  alcohol();
  lcd.clear();
}

void updatew()
{
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(alco_level) +"&"+ field2 +"="+String(fuel_level);
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
