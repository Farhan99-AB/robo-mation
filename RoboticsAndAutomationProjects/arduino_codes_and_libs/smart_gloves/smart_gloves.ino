#include "SD.h"                      // need to include the SD library
#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328
#include "TMRpcm.h"          //  also need to include this library...
#include "SPI.h"
TMRpcm tmrpcm;   // create an object for use in this sketch
#include <LiquidCrystal.h>

const int rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
 int SW0;
 int SW1; 
 int SW2;
 int SW3;
 int SW4;
void setup()
{
 lcd.begin(16, 2);
 lcd.print("Smart Gloves");
 delay(2000);
 lcd.clear();
 pinMode(A0,INPUT); 
 pinMode(A1,INPUT);  //Define A0 as digital input.
 pinMode(A2,INPUT);  //Define A1 as digital input. 
 pinMode(A3,INPUT);  //Define A2 as digital input.
 pinMode(A4,INPUT);  //Define A3 as digital input.
 
 tmrpcm.speakerPin = 9; //11 on Mega, 9 on Uno, Nano, etc
 Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
  Serial.println("SD fail");
  return;   // don't do anything more if not
  }
 tmrpcm.volume(20);
 tmrpcm.play("hello.wav"); //the sound file "1" will play each time the arduino powers up, or is reset
 delay(5000);
}
 
void loop()
{  
 SW0=analogRead(A0);
 Serial.print(SW0);
 SW1=analogRead(A1);
 Serial.print(" ");
 Serial.print(SW1);
 SW2=analogRead(A2);
 Serial.print(" ");
 Serial.print(SW2);
 SW3=analogRead(A3);
 Serial.print(" ");
 Serial.print(SW3);
 SW4=analogRead(A4);
 Serial.print(" ");
 Serial.print(SW4);
 Serial.println();
 
    if(tmrpcm.isPlaying() == 1)
    {
    }
    else
    {
      if (SW0 > 1005) 
      {
        tmrpcm.play("1.wav");
        lcd.setCursor(4, 0);
        lcd.print("GOOD BYE");
        delay(1500);
        lcd.clear();
      }
      else if (SW1 > 1005) 
      { //if SW1 pressed then play file "6.wav"
        tmrpcm.play("2.wav");
        lcd.setCursor(3, 0);
        lcd.print("WELCOME");
        delay(1500);
        lcd.clear();
      } 
      else if(SW2 > 1005)
      { //if SW2 pressed then play file "4.wav"
        tmrpcm.play("3.wav");
        lcd.setCursor(3, 0);
        lcd.print("THANK YOU");
        delay(1500);
        lcd.clear();
      } 
      else if(SW3 > 1005)
      { //if SW3 pressed then play file "5.wav"
        tmrpcm.play("4.wav");
        lcd.setCursor(3, 0);
        lcd.print("GREETINGS");
        delay(1500);
        lcd.clear();
      } 
      else if(SW4 > 1005)
      {  //if SW4 pressed then play file "3.wav"
        tmrpcm.play("5.wav");
        lcd.setCursor(4, 0);
        lcd.print("HELLO!!!");
        delay(1500);
        lcd.clear();
      }
    }
}
