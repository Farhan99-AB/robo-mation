#define ldr1 A0
#define ldr2 A1
#define ldr3 A2
#define r1 2 
#define r2 3
#define r3 10
#define lit A4

#include <LiquidCrystal.h>
LiquidCrystal lcd(9,8,7,6,5,4);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//moist sens
int moistpin = A3; 
int moist_Value;  
int limit = 300;

int tol=40,v1,v2,v3;

void setup() 
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("BEC College");
  delay(2000);
  lcd.clear();
  lcd.print("Automatic");
  lcd.setCursor(0,1);
  lcd.print("Irrigation");
  delay(2000);
  lcd.clear();
  pinMode(r1,OUTPUT);
  pinMode(r2,OUTPUT);
  pinMode(r3,OUTPUT);
  pinMode(lit, OUTPUT);
  pinMode(ldr1,INPUT_PULLUP);
  pinMode(ldr2,INPUT_PULLUP);
  pinMode(ldr3,INPUT_PULLUP);
}

void loop() 
{
  solar_tracker();
  light();
  moist();
}

void solar_tracker()
{
  v1=analogRead(ldr1);
  v2=analogRead(ldr2);

  Serial.print(v1);
  Serial.print(" ");
  Serial.print(v2);
  Serial.print(" ");
  
  if ((v1>v2)&&(abs(v1-v2)>tol))
  {
    digitalWrite(r1,HIGH);
    digitalWrite(r2,LOW);
    Serial.println("CCW");
  }
  else if ((v1<v2)&&(abs(v1-v2)>tol))
  {
    digitalWrite(r1,LOW);
    digitalWrite(r2,HIGH);
    Serial.println("CW");
  }
  else
  {
    digitalWrite(r1,LOW);
    digitalWrite(r2,LOW);
    Serial.println("");
  }
}

void light()
{
  v3=analogRead(ldr3);
  Serial.println(v2);
  v3 = map(v3, 0, 1023, 0, 255);
  analogWrite(lit, v3);  
}

void moist()
{
  moist_Value = analogRead(moistpin);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOISTURE: ");
  lcd.setCursor(10, 0);
  lcd.print(moist_Value);
  delay(1000);
  lcd.clear(); 
  Serial.print("Mosit Value : ");
  Serial.println(moist_Value);
  if(moist_Value > 600)
  {
    Serial.println("MOTOR ONN"); 
    digitalWrite(r3,HIGH);
    lcd.clear();
    lcd.print("MOTOR ONN");
    delay(1500);
    lcd.clear();
  }
  else
  {
    Serial.println("MOTOR OFF");
    digitalWrite(r3,LOW);
    lcd.clear();
    lcd.print("MOTOR OFF");
    delay(1500);
    lcd.clear();
  }
}
