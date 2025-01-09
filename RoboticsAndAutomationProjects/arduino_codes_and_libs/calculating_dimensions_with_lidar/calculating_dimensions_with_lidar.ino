#include <LiquidCrystal.h>
LiquidCrystal lcd(7,8,9,10,11,12);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

#include <Wire.h>
#include <VL53L0X.h>
#include <NewPing.h>

#define TRIGGER_PIN  3
#define ECHO_PIN     2
#define MAX_DISTANCE 400

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

VL53L0X sensor;
VL53L0X sensor2;
VL53L0X sensor3;

int a;
int b;
int c;

int a1;
int b1;
int c1;

int distance;

int len;
int bth;
int hgt;


void setup()
{

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);

  delay(500);
  Wire.begin();


  Serial.begin (9600);

  
  lcd.begin(16, 2);
  lcd.print("Dimension");
  lcd.setCursor(0,1);
  lcd.print("Measurement");
  delay(2000);
  lcd.clear();


  digitalWrite(4, HIGH);
  delay(150);
  Serial.println("00");
  sensor.init(true);

  Serial.println("01");
  delay(100);
  sensor.setAddress((uint8_t)01);
  Serial.println("02");

  digitalWrite(5, HIGH);
    delay(150);
  sensor2.init(true);
  Serial.println("03");
  delay(100);
  sensor2.setAddress((uint8_t)02);
  Serial.println("04");
  
  digitalWrite(6, HIGH);
    delay(150);
  sensor3.init(true);
  Serial.println("05");
  delay(100);
  sensor3.setAddress((uint8_t)03);
  Serial.println("06");
  


  Serial.println("addresses set");

  sensor.startContinuous();
  sensor2.startContinuous();
  sensor3.startContinuous();

  a=sensor.readRangeContinuousMillimeters();
  b=sensor.readRangeContinuousMillimeters();
  c=sensor.readRangeContinuousMillimeters(); 
  Serial.print("Length:");
  Serial.println(a);
  Serial.print("Height:");
  Serial.println(b);
  Serial.print("Breadth");
  Serial.println(c);
  delay(3000);

}

void loop()
{
  distance = sonar.ping_cm();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  delay(200);
  if(distance>10 && distance<20)
  {
    a1=sensor.readRangeContinuousMillimeters();
    Serial.print(a1);
    Serial.print("  ");
   
    b1=sensor2.readRangeContinuousMillimeters();
    Serial.print(b1);
    Serial.print("  ");
    
    c1=sensor3.readRangeContinuousMillimeters();
    Serial.println(c1);
    
    len = (a-a1);
    Serial.print("Length:");
    Serial.println(len);
    
    hgt = (b-b1) - 20;
    Serial.print("Breadth:");
    Serial.println(hgt);
    
    bth = (c-c1) + 20;
    Serial.print("Height:");
    Serial.println(bth);
    
    lcd.print("Length: ");
    lcd.setCursor(8,0);
    lcd.print(len);
    delay(2000);
    lcd.clear();

    lcd.print("Height: ");
    lcd.setCursor(8,0);
    lcd.print(bth);
    delay(2000);
    lcd.clear();

    lcd.print("Breadth: ");
    lcd.setCursor(8,0);
    lcd.print(hgt);
    delay(2000);
    lcd.clear();
  }
}
