#include <Ultrasonic.h>
Ultrasonic ultrasonic(38, 40);   //(trig,echo)
int distance=0;

#include <SoftwareSerial.h>
SoftwareSerial ser(2, 3);              // RX, TX

#include <Keypad.h>

#include <LiquidCrystal.h>
LiquidCrystal lcd(42,44,46,48,50,52);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

// IOT 1
String apiKey = "IG1QN7SQ45TR7UB5";    // Edit this API key according to your Account
String Host_Name = "GP13";                                                                                  // Edit Host_Name
String Password = "ganeshpatil";                                                                            // Edit Password

String cmd;

#define alco 15
int alco_level=0;

#define limit 37
int limit_state=0;

#define float_sens 40
int fuel_level=0;

#define main_relay 19

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
byte rowPins[ROWS] = {39, 41, 43, 45}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {47, 49, 51, 53}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() 
{
  ser.begin(115200);
  ser.println("AT+RST");               // Resetting ESP8266
  
  char inv = '"';
  String cmd = "AT+CWJAP";

  cmd += "=";
  cmd += inv;
  cmd += Host_Name;
  cmd += inv;
  cmd += ",";
  cmd += inv;
  cmd += Password;
  cmd += inv;
  ser.println(cmd);                    // Connecting ESP8266 to your WiFi Router



  Serial.begin(9600);   // Initiate a serial communication
  
  pinMode(limit,INPUT_PULLUP);
  pinMode(float_sens,INPUT_PULLUP);
  pinMode(main_relay,OUTPUT);

  lcd.begin(16, 2);
  lcd.print("SMART HELMET");
  delay(2000);
  lcd.clear();

  key_pad();
}
void loop() 
{
  //EstablishTCPconnection();
  //UpdatetoTS();
  //fuel();
  //alcohol();
  //helmet();
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
            helmet();
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
  fuel_level=digitalRead(float_sens);
  if(fuel_level==0)
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
  limit_state=digitalRead(limit);
  while(digitalRead(limit)==1)
  {
    lcd.print("WEAR HELMET!!!!!!");
    delay(500);
    lcd.clear();
    delay(500);
  }
  lcd.print("HELMET ON");
  delay(2000);
  alcohol();
  lcd.clear();
}

void EstablishTCPconnection()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";          // Establishing TCP connection
  cmd += "184.106.153.149";                       // api.thingspeak.com
  cmd += "\",80";                                 // port 80

  ser.println(cmd);
  Serial.println(cmd);
  if (ser.find("Error")) 
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}

void UpdatetoTS()
{
  String state1 = String(distance);               // Converting them to string as to send it through URL
  String state2 = String(limit_state);
  String state3 = String(alco_level);
  String state4 = String(fuel_level);


  String getStr = "GET /update?api_key=";         // prepare GET string
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(state1);
  getStr += "&field2=";
  getStr += String(state2);
  getStr += "&field3=";
  getStr += String(state3);

  getStr += "\r\n\r\n";
  
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());                // Total Length of data

  ser.println(cmd);
  Serial.println(cmd);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connection");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("data sent");
  delay(1000);
  lcd.clear();

  delay(1000);

  if (ser.find(">")) 
  {
    ser.print(getStr);
    Serial.print(getStr);
  }
  else 
  {
    ser.println("AT+CIPCLOSE");                  // closing connection
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
  delay(1000);                                  // Update after every 15 seconds 
}
  
