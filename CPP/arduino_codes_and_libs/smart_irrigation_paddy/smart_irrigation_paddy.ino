#define r3 10

#include <LiquidCrystal.h>
LiquidCrystal lcd(9,8,7,6,5,4);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);

//moist sens
int moistpin = A0; 
int moist_Value;  
int limit = 300;

//water sens
int water;
int water_sens = 3;

//temp
int lm35 = A2;
int temp;

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
  pinMode(r3,OUTPUT);
  pinMode(water_sens,INPUT_PULLUP);
}

void loop() 
{
  moist();
  temp1();
  check_water();
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
}

void temp1()
{
  temp=analogRead(lm35);
  int temp_val = (temp * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10);                                                                      
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  Serial.print("Temperature:");
  lcd.setCursor(13, 0);
  lcd.print(temp_val);
  Serial.println(temp_val);
  delay(1000);
  lcd.clear();
}

void check_water()
{
     water = digitalRead(water_sens);
     Serial.print("Water Level: ");
     Serial.println(water);
     lcd.setCursor(0, 0);
     lcd.print("Water Level: ");
     lcd.setCursor(13, 0);
     lcd.print(water);
     delay(2000);
     lcd.clear();
     if(water == 0)
     {
      Serial.println("MOTOR ONN");
      digitalWrite(r3,HIGH);
     }
     else
     {
      Serial.println("MOTOR OFF");
      digitalWrite(r3,LOW);
     }
}
