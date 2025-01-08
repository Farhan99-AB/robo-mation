#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(41,39,37,35,33,31);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

// IOT 1
      String apiKey = "3OGIRFGGQ89I4L74";    // Edit this API key according to your Account
      String Host_Name = "SATURDAY";                                                                                  // Edit Host_Name
      String Password = "SATURDAY";                                                                            // Edit Password4
      
SoftwareSerial ser(0,1);              // RX, TX

int i = 1;
String cmd;

//////////RFID
#define SS_PIN 53
#define RST_PIN 22
//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define smoke_sens 2
#define water_spray 15
#define buzzer 16

#define limit 17
int limit_state;

#define ldr 0        //A0
#define light 18

#define lm35 1       //A1
#define main_switch 19

int state=0;
unsigned long start_time=0;

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
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() 
{

  
// IOT 2
  Serial.begin(115200);                  // enable software serial
  ser.begin(115200);
  ser.println("AT+RST");               // Resetting ESP8266

    Serial.begin(115200);
  
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
  
//  SPI.begin();      // Initiate  SPI bus
  
//  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  pinMode(limit,INPUT_PULLUP);
  pinMode(water_spray,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(light,OUTPUT);

  lcd.begin(16, 2);
}

void loop() 
{
  EstablishTCPconnection();
  UpdatetoTS();
  //rfid();
  //smoke();
  //cnc_door();
  //auto_light();
  //temperature();
  //current();
//  if(millis-start_time>10000)
//  {
//    digitalWrite(main_switch,LOW);
//  }
}

void key_pad()
{
  lcd.clear();
  Serial.println("Enter Your Password");
  lcd.print("Enter password: ");
  String password="12345";
  String input_password;
  char code[5];
  int i=0;
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
      } 
      else if(key == '#') 
      {
        if(input_password == password) 
        {
            if(state==0)
            {
              Serial.println("password is correct");
              digitalWrite(main_switch,HIGH);
              //start_time=millis();
              Serial.println("MAINS ON");
              lcd.clear();
              lcd.print("MAINS ON");
              state=1;
              break;
            }
            if(state==1)
            {
              Serial.println("password is correct");
              digitalWrite(main_switch,LOW);
              //start_time=millis();
              Serial.println("MAINS OFF");
              lcd.clear();
              lcd.print("MAINS OFF");
              state=0;
              break;
            }
        } 
        else 
        {
          Serial.println("Entered Password: ");
          Serial.println(input_password);
          Serial.println("password is incorrect, try again");
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
//void rfid()
//{
//  lcd.clear();
//    // Look for new cards
//  if ( ! mfrc522.PICC_IsNewCardPresent()) 
//  {
//    return;
//  }
//  // Select one of the cards
//  if ( ! mfrc522.PICC_ReadCardSerial()) 
//  {
//    return;
//  }
//  //Show UID on serial monitor
//  Serial.print("UID tag :");
//  String content= "";
//  byte letter;
//  for (byte i = 0; i < mfrc522.uid.size; i++) 
//  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
//     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//     content.concat(String(mfrc522.uid.uidByte[i], HEX));
//  }
//  Serial.println();
//  Serial.print("Message : ");
//  content.toUpperCase();
//  if (content.substring(1) == "B4 58 90 79") //change here the UID of the card/cards that you want to give access
//  {
//    Serial.println("Authorized access");
//    lcd.print("Authorized access");
//    delay(1000);
//    key_pad();
//    Serial.println();
//    delay(3000);
//  }
//  else   
//  {
//    Serial.println(" Access denied");
//    lcd.print(" Access denied");
//    delay(3000);
//  }
//  lcd.clear();
//}

void smoke()
{
  lcd.clear();
  Serial.print("Smoke: ");
  Serial.println(analogRead(2));
  if(analogRead(smoke_sens)>200)
  {
    
    digitalWrite(water_spray,HIGH);
    digitalWrite(buzzer,HIGH);
    Serial.println("FIRE!!!!!!!!!!!!!!!!!!!!");
    lcd.print("FIRE!!!!!!!!!");
    delay(1000);
  }
  else
  {
    digitalWrite(water_spray,LOW);
    digitalWrite(buzzer,LOW);
    lcd.clear();
  }
}

void cnc_door()
{
  lcd.clear();
  limit_state=digitalRead(limit);
  Serial.println(limit_state);
  if(!limit_state)
  {
    lcd.print("CNC DOOR OPEN");
  }
  else
  {
    lcd.print("CNC DOOR CLOSED");
  }
  delay(2000);
}

void auto_light()
{
  lcd.clear();
  if(analogRead(ldr)>500)
  {
    digitalWrite(light,HIGH);
    Serial.println("LIGHT OFF");
    lcd.print("LIGHT OFF");
  }
  else
  {
    digitalWrite(light,LOW);
    Serial.println("LIGHT ON");
    lcd.print("LIGHT ON");
  }
  delay(2000);
}

void temperature()
{
  lcd.clear();
  int temp=analogRead(lm35)*0.488;
  Serial.print("Temperature: ");
  Serial.println(temp); 
  lcd.print("Temperature: ");
  lcd.print(temp);
  lcd.print("C");
  delay(2000);
}

void current()
{
  lcd.clear();
  Serial.print("Current: ");
  Serial.println(analogRead(4)); 
  lcd.print("Current: ");
  lcd.print(analogRead(4));
  delay(2000);
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
  int temp=analogRead(lm35)*0.488;
  int limit_state=digitalRead(limit);
  int smoke=digitalRead(smoke_sens);
  float volt=analogRead(ldr);
  volt = map(volt,0,1024,0,15);
  

  String state1 = String(temp);               // Converting them to string as to send it through URL
  String state2 = String(limit_state);
  String state3 = String(smoke);
  String state4 = String(volt);
  String state5 = String(volt);


  String getStr = "GET /update?api_key=";         // prepare GET string
  getStr += apiKey;
  getStr += "&field1=";
  getStr += String(state1);
  getStr += "&field2=";
  getStr += String(state2);
  getStr += "&field3=";
  getStr += String(state3);
  getStr += "&field4=";
  getStr += String(state4);
  getStr += "&field5=";
  getStr += String(state5);


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
  
