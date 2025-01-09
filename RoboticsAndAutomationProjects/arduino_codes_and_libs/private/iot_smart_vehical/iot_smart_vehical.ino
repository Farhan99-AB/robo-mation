#include <SoftwareSerial.h>     // LIBERY
#include <LiquidCrystal.h>      // LIBERY
#include <Ultrasonic.h>         // LIBERY

 #define limit_s A1         
  #define sensorAnalog A0
    int alco_level=0;
      int value;
    int limit_state;
    // temperature
        int cel;
        int val;
        int lm35 = A4;


  const int relay=12;
        
String apiKey = "7CD7M1WHMEAQAM8Z";    // Edit this API key according to your Account
String Host_Name = "FUTURE TECHNOLOGYS";         // WIFA Host_Name
String Password = "FUTURE TECHNOLOGYS";          // WIFA Password

SoftwareSerial ser(11, 12);              // RX, TX    ESP8266 PIN 

int i = 1;
String cmd;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

// ultrasonic
//Ultrasonic ultrasonic(8,9);     //(TRIG,ECHO) ULTRASONIC PIN 
int distance;

void setup()
{

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


  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.print("       SGBIT");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("    COLLEGE");                //SECOND LINE
  delay(3000);
  lcd.clear();
 
  lcd.print("      E/C");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("    DEPARTMENT");   
  delay(3000);
  lcd.clear();
  
    lcd.print("Smart & Safety ");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("vehical with IOT");                //SECOND LINE
  delay(3000);
  lcd.clear();

            pinMode(relay, OUTPUT);
            pinMode(limit_s,INPUT_PULLUP);
          

}

void loop()
        {
           helmet();
           alcohol();
           temp1();
           EstablishTCPconnection();
           UpdatetoTS();
        }


                void helmet()
                        {
                          lcd.clear();
                          limit_state=digitalRead(limit_s);
                          while(digitalRead(limit_s)==1)
                          {
                            lcd.print("WEAR HELMET!!!!!!");
                            digitalWrite(relay,LOW);
                            delay(500);
                            lcd.clear();
                            delay(500);
                          }
                          lcd.print("HELMET ON");
                           digitalWrite(relay,HIGH);
                          delay(2000);
                          lcd.clear();
                        }

void alcohol()
        {
          lcd.clear();
          
          alco_level=analogRead(sensorAnalog);
              lcd.setCursor(1,0);
              lcd.print("Alcohol L: ");
                   lcd.print(alco_level);
                   delay(2000);
                    lcd.clear();
                   
          if(alco_level>500)
          {
            digitalWrite(relay,LOW);
            lcd.setCursor(0,1);
            lcd.print("Exceeded:");
             lcd.print(alco_level);
            delay(2000);
            lcd.clear();
        
           }
        
          else
          {
               lcd.clear();
             lcd.setCursor(0,1);
             lcd.print("NO Alcohol: ");
              digitalWrite(relay,HIGH);
            delay(2000);
           lcd.clear();
             
         }
        }

  void temp1()
          {
            int temp=analogRead(lm35)*0.488;                                                                         
                 lcd.setCursor(1, 0);
                 lcd.print("Temperature");
                 lcd.setCursor(0, 1);
                 lcd.print(temp);
                 delay(1000);
                   lcd.clear();
          
           if(temp>37)                                                                     
            {
              lcd.setCursor(1, 0);
             lcd.print("ENGINE HEATED");
              digitalWrite(relay,LOW);
              delay(2000);
              lcd.clear();
            }
            else
            {
              digitalWrite(relay, HIGH);
              delay(3000);
              lcd.clear();   
            }
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
alco_level=analogRead(sensorAnalog);
limit_state=digitalRead(limit_s);

  String state1 = String(limit_state);               // Converting them to string as to send it through URL
  String state2 = String(alco_level);
  String state3 = String(temp);

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
  
