#include <SoftwareSerial.h>     // LIBERY
#include <LiquidCrystal.h>      // LIBERY
#define USE_ARDUINO_INTERRUPTS true

#include <PulseSensorPlayground.h>

#include <dht.h>
#define dht_apin A4 // Analog Pin sensor is connected to 
dht DHT;

String apiKey = "ZTG06Z62YQ68K4BF";    // Edit this API key according to your Account
String Host_Name = "SATURDAY";         // WIFA Host_Name
String Password = "SATURDAY";          // WIFA Password

SoftwareSerial ser(8, 9);              // RX, TX    ESP8266 PIN 

int i = 1;
String cmd;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10,11,12);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

const int PulseWire = A0;
int Threshold = 550;
PulseSensorPlayground pulseSensor;


void setup()
{

  Serial.begin(115200);                  // enable software serial
  ser.begin(115200);
  ser.println("AT+RST");               // Resetting ESP8266

    Serial.begin(115200);
   Serial.println("DHT11 Humidity & temperature Sensor\n\n");
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
  
  lcd.print("  KLE");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("    COLLEGE");                //SECOND LINE
  delay(3000);
  lcd.clear();
 
  lcd.print("    EC ");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("    DEPARTMENT");   
  delay(3000);
  lcd.clear();
  
  lcd.print("HEALTH CARE");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print(" MONITORING");                //SECOND LINE
  delay(3000);
  lcd.clear();


  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);

  if (pulseSensor.begin()) 
    Serial.println("PulseSensor object created!");
  
}

void loop()
{
  print_data_on_lcd();
  values();
  heartrate();
  EstablishTCPconnection();
  UpdatetoTS();
}

void print_data_on_lcd()

{
  lcd.print("The Patient is being");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("MONITORING");                //SECOND LINE
  delay(3000);
  lcd.clear();
}

void values()
{

    DHT.read11(dht_apin);
    
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
//        DHT.read11(dht_apin);
    lcd.setCursor(1, 0);
    lcd.print("humid=");
    lcd.print(DHT.humidity);
    lcd.print("%  ");
    lcd.setCursor(0, 1);
    lcd.print("temp= ");
    lcd.print(DHT.temperature);
    lcd.print("C  ");
    delay(2000);
    lcd.clear();
} 

void heartrate()
{
  int myBPM = pulseSensor.getBeatsPerMinute();          
  if (pulseSensor.sawStartOfBeat()) 
  {                   
    Serial.println("♥  A HeartBeat Happened ! "); 
    Serial.print("BPM: ");
    Serial.println(myBPM);                        
    lcd.setCursor(0, 0);
    lcd.print("HEART RATE");
    lcd.setCursor(0, 1);
    lcd.print(myBPM);
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
  DHT.read11(dht_apin);
  int myBPM = pulseSensor.getBeatsPerMinute(); 


 String state1 = String(DHT.humidity);               // Converting them to string as to send it through URL
  String state2 = String(myBPM);
 String state3 = String(DHT.temperature);

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
  delay(2000);
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
  
