#include <SoftwareSerial.h>
#include <HCSR04.h>              //Include ultrsonic Sensor library

SoftwareSerial HC12(9, 10); // HC-12 TX Pin, HC-12 RX Pin
//ultrasense
HCSR04 hc(12, 13); //initialisation class HCSR04 (trig pin , echo pin)
int th;

#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,11);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

#include <SoftwareSerial.h>
SoftwareSerial ser(2, 3);              // RX, TX

// IOT 1
String apiKey = "ICNP1FIRT49PNWHH";    // Edit this API key according to your Account
String Host_Name = "OnePlus 7";                                                                                  // Edit Host_Name
String Password = "12345678911";

String cmd;
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
  Serial.begin(9600);             // Serial port to computer
 // HC12.begin(9600);               // Serial port to HC12
  Serial.println("Radio ONN");
  pinMode(capteur_A, INPUT);
  pinMode(ph_sense, INPUT);
  lcd.begin(16, 2);
  lcd.print("Smart Irrigation");
  delay(2000);
  lcd.clear();
  

}

void loop() 
{
  check_ultrasense();
  temp1();
  rain();
  moist();
  ph();
  EstablishTCPconnection();
  UpdatetoTS();
    if((moist_Value > 400))
    {
      Serial.println("START");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: ONN");
      int text = 1;
      //HC12.write(text);                  //Sending the message to receiver
      delay(1500);
    }
    else
    {
      Serial.println("STOP");
      lcd.setCursor(0,1);
      lcd.print("MOTOR STATE: OFF");
      int text = 0;
      //HC12.write(text);
      delay(1500);
    }
    //radio.write(&button_state, sizeof(button_state));  //Sending the message to receiver 
    delay(10);
  //}
}

void check_ultrasense()
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
}

void temp1()
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
}

void rain()
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
}

void moist()
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

void ph()
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
  String state1 = String(th);               // Converting them to string as to send it through URL
  String state2 = String(val_analogique);
  String state3 = String(temp);
  String state4 = String(moist_Value);
  String state5 = String(ph_Value);


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
  lcd.clear();
  lcd.setCursor(0, 0);
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
