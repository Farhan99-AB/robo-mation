/*
 * Project title: Border Alert Security

 */
#include<NewPing.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
const int _buzzer = 13;
const int _motor_One = 8;
const int _motor_Two = 9;
 int acc = 12;
LiquidCrystal lcd(2,3,4,5,6,7);
SoftwareSerial mySerial(10,11);
void setup() {
  // put your setup code here, to run once:
pinMode(_buzzer, OUTPUT);

pinMode(_motor_One, OUTPUT);
pinMode(_motor_Two, OUTPUT);


digitalWrite(_motor_One, LOW);
digitalWrite(_motor_Two, LOW);

lcd.begin(16,2);
startup();
Serial.begin(9600);
mySerial.begin(9600);
digitalWrite(_motor_One, HIGH);
}
void loop()

{
 zone();
 digitalWrite(_motor_One, HIGH);
    //startup();
}






void zone()

{

if(Serial.available() > 0)
{
char _getChar = Serial.read();
if(_getChar == 'A')

{
     digitalWrite(_buzzer, HIGH);
 digitalWrite(_motor_Two, HIGH);
digitalWrite(_motor_One, LOW);
display_LCD("SCHOOL ZONE ON","SPEED SLOW");
  delay(2000);
 _getChar=0;

}
else if(_getChar == 'B')
{
     digitalWrite(_buzzer, HIGH);
  digitalWrite(_motor_Two, HIGH);
  digitalWrite(_motor_One, LOW);
display_LCD("HOSPITAL DETECT","SPEED SLOW");
  delay(2000);
//send_SMS(" BOAT NO. 123 HAS CROSSED FIRST ZONE OF BOARDER, PLS ALERT THE BOAT  ");

 _getChar=0;
}

}

if(digitalRead(acc) == HIGH)
  {
    send_SMS("Vehicle NO. KA 7823 HAS Occured with an Accident, Please Arrive for help Imediately");
       delay(1000);
     send_SMS1("Vehicle NO. KA 7823 HAS Occured with an Accident, Please Arrive for help Imediately");
    delay(1000);
    digitalWrite(_buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Accident has");
    lcd.setCursor(0,1);
    lcd.print("  OCCURED ");
    delay(3000);
      lcd.clear();
     lcd.setCursor(0,0);
    lcd.print("QUICK SMS SENT");
    delay(2000);
    digitalWrite(_buzzer, LOW);
   
   
  }
  startup();
}





void startup()
{
  lcd.clear();
lcd.setCursor(0,0);
lcd.print(" VEHICLE SMART ");
lcd.setCursor(0,1);
lcd.print("  ZONE SYSTEM  ");
delay(1000);
}
void display_LCD(String msg_One,String msg_Two){
lcd.clear();
lcd.setCursor(0,0);
lcd.print(msg_One);
lcd.setCursor(0,1);
lcd.print(msg_Two);
delay(1000);
}


void send_SMS(char *msg)

{

mySerial.println("AT");
delay(1000);
mySerial.println("AT+CMGF=1");
delay(500);
mySerial.println("AT+CMGS=\"9482497756\"\r");
delay(1000);
mySerial.println(msg);
delay(500);
Serial.println(0x1A);
mySerial.println(char(26));
delay(1000);

}



void send_SMS1(char *msg)

{

mySerial.println("AT");
delay(1000);
mySerial.println("AT+CMGF=1");
delay(500);
mySerial.println("AT+CMGS=\"7259412003\"\r");
delay(1000);
mySerial.println(msg);
delay(500);
Serial.println(0x1A);
mySerial.println(char(26));
delay(1000);

}
