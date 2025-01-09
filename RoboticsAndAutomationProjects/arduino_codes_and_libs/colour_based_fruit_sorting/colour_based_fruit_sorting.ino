#include <Wire.h> 
#include <LiquidCrystal.h>      // LIBERY
#include <Servo.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 13, 4, 5, 6, 7);
//LiquidCrystal lcd(rs, en, D4, D5, D6, D7);
int i = 1;
int red = 0;  
int green = 0;  
int yellow = 0;  
int limit;
Servo myservo;
    
void setup()   
{  
  Serial.begin(9600); 
  pinMode(3, OUTPUT);  //edited
  pinMode(8, OUTPUT);  //
  pinMode(9, OUTPUT);  
  pinMode(12, OUTPUT);  
  pinMode(11, OUTPUT);  
  pinMode(10, INPUT);
  myservo.attach(3);
        
  digitalWrite(8, HIGH);  
  digitalWrite(9, HIGH); 
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.print(" fruit");           //FIRST LINE
  lcd.setCursor(0, 1);
  lcd.print("sorting");                //SECOND LINE
  delay(3000);
  lcd.clear();
}  
    
void loop() 
{  
  myservo.write(0);
  delay(3000);
  digitalWrite(12, LOW);  
  digitalWrite(11, LOW);  
  red = pulseIn(10, digitalRead(10) == HIGH ? LOW : HIGH); 
  Serial.print("RED");
  Serial.print(red); 
  digitalWrite(11, HIGH);  
  yellow = pulseIn(10, digitalRead(10) == HIGH ? LOW : HIGH);
  Serial.print("        YELLOW");
  Serial.print(yellow);  
  digitalWrite(12, HIGH);  
  green = pulseIn(10, digitalRead(10) == HIGH ? LOW : HIGH);
  Serial.print("         GREEN");
  Serial.println(green);   
 
  lcd.clear();
  myservo.write(0);
  if (red < 15 && red < green && green > 15 )
  {  
   Serial.println(" Red Color"); 
   limit = 140;
   servo(limit); 
   lcd.setCursor(1, 0);
   lcd.print("Red Color");
   delay(1000);
    lcd.clear();
  }  
  else if ((yellow < red) && (yellow < green)&& yellow > 5)    
  {  
   //Serial.println("No Colour");
   //limit = 60;  
   //servo(limit);
   lcd.setCursor(1, 0); 
   //lcd.print("No Color");
    //delayu(1000); 
    //lcd.clear();
  }  

  else if ((green > red) && (green > yellow))  
  {  
   Serial.println(" Green Color");
   limit = 100;  
   servo(limit); 
   lcd.setCursor(1, 0); 
   lcd.print("Green Color");
     delay(1000);
     lcd.clear();
  }  
  delay(500);   
}

int servo(int limit)
{
    myservo.write(limit);
    delay(30);
//    if(limit == 120)
//    {
//      limit = 0;  
//    }
}
