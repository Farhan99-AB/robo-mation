#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

#include <Servo.h>

// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Include Keypad library
#include <Keypad.h>

#include <HCSR04.h>
HCSR04 hc(13,12);
int th;

Servo myservo;  // create servo object to control a servo
Servo myservo2;
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int e_w;

//pins:
const int HX711_dout = 52; //mcu > HX711 dout pin
const int HX711_sck = 53; //mcu > HX711 sck pin

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

int lc_weight;

// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

char data[6];
int data_count = 0;

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2); 

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(57600); 
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  lcd.backlight();
  lcd.init(); 
  lcd.print("ONN");
  delay(1000);
  lcd.clear();
  myservo.attach(10);
  myservo.write(170);
  myservo2.attach(11);
  myservo2.write(10);

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
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() 
{
//  // receive command from serial terminal, send 't' to initiate tare operation:
//  if (Serial.available() > 0) {
//    char inByte = Serial.read();
//    if (inByte == 't') LoadCell.tareNoDelay();
//  }

//  // check if last tare operation is complete:
//  if (LoadCell.getTareStatus() == true) {
//    Serial.println("Tare complete");
//  }

  
  // LCD AND KEYPAD INTERFACE AND SERVO OPEN
  e_w = ent_weight();
  servo_open();

  //LOAD CELL INTERFACE
  loadcell();

  //ultrasonic and servo 2
  ultrasense();
   
}

void servo_open()
{
  Serial.println("Servo Open");
    myservo.write(140);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
}

void servo_close()
{
  Serial.println("Servo Close");
    myservo.write(170);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
}

void loadcell()
{
  Serial.println("Measuring Weight");
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
        Serial.print("Entered weight: ");
        Serial.println(e_w);
        //CLOSE SERVO AND LOAD CELL 
        if(lc_weight >= e_w)
        {
          servo_close();
          break;
        }
      }
    }
  }
}

int ent_weight()
{
  // Get key value if pressed
  int val;
  Serial.print("Enter weight:");
  while(1)
  {
    char customKey = customKeypad.getKey();
    if (customKey){
      // Clear LCD display and print character
      data[data_count] = customKey;
      lcd.print(data[data_count]);
      Serial.print(data[data_count]);
      data_count++;
    }
    if(customKey == '#')
    {
      Serial.println();
      Serial.print("Weight = ");
      val = print_data();
      Serial.println("gms");
      clear_data();
      break;
    }
  }
  Serial.print("val = ");
  Serial.println(val);
  return val;
}

void clear_data()
{
  while (data_count != 0) 
    {
      data[data_count--] = 0;
    }
}

int print_data()
{
  int i,j,w[5],m=0,k;
  char a[10]={'0','1','2','3','4','5','6','7','8','9'};
  char val[5];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight= ");
  lcd.setCursor(8,0);
  for(i=0; i<data_count-1; i+=1)
  {
    Serial.print(data[i]);
    lcd.print(data[i]);
    for(j=0;j<11;j+=1)
    {
      if(data[i] == a[j])
      {
        m=m+j;
        m=m*10;
        break;
      }
    }
  }
  m=m/10;
  int n = 8+i;
  lcd.setCursor(n,0);
  lcd.print(" gms");
  delay(2000);
  lcd.clear();
  Serial.print("Weight:");
  Serial.println(m);
  return m;
}

void ultrasense()
{
  while(1)
  {
    th = hc.dist();
    Serial.print("Ultra sens: ");
    Serial.println( th ); //return current distance (cm) in serial
    delay(60);                   // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
    if(th > 10 && th < 20)
    {
      myservo2.write(30);
      Serial.println("Servo2 open");
      delay(5000);
      myservo2.write(10);
      Serial.println("Servo2 close");
      break;
    }
  }
}
