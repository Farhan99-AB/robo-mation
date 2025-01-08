#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7) 

#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);

String rfidCard;

//motor driver
const int enA = 9;
const int lf_motor = 3;
const int lb_motor = 4;

void setup() 
{
  Serial.begin(9600);
   pinMode(enA, OUTPUT);
   pinMode(lf_motor, OUTPUT);
   pinMode(lb_motor, OUTPUT); 
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
}

void loop() 
{
  if (rfid.isCard()) 
  {
    if (rfid.readCardSerial()) 
    {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      if(rfidCard=="137 51 119 233")
      {
        Serial.println("In zone slowing speed");
        analogWrite(enA, 100);
        digitalWrite(lf_motor, HIGH);
        digitalWrite(lb_motor, LOW);
        lcd.print("Sending Location");
        //loc();
        lcd.clear();
        lcd.print("Location Sent");
        delay(1000);
        lcd.clear(); 
      }
    }
}
}
