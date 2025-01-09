#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Creates an LCD object. 
//Parameters: (rs, enable, d4, d5, d6, d7) 

#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);

String rfidCard;

#define charger 8

int u1_bal = 100;

long int stime=0;


void setup() 
{
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();

  pinMode(charger,OUTPUT);
  digitalWrite(charger,LOW);

  lcd.begin(16,2);

  lcd.print("RFID CHARGING"); 
  delay(3000); 
  lcd.setCursor(0,1); 
  lcd.print("Machine ON");
  delay(1000);  
}

void loop() 
{
  if (rfid.isCard()) 
  {
    if (rfid.readCardSerial()) 
    {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);

      ///////////////////////////////////////////////////////////////////////////////USER1
      if(rfidCard=="115 125 148 49")
      {
        Serial.println("USER1 charging");
        digitalWrite(charger,HIGH);
        lcd.clear();
        lcd.print("User1 charging");  
        lcd.setCursor(0,1); 
        lcd.print("Rem balance: ");
        lcd.print(u1_bal);
        while(u1_bal>0)
        {
          u1_bal=u1_bal-1;
          if (rfid.isCard()) 
          {
            if (rfid.readCardSerial()) 
            {
              rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
              if(rfidCard=="115 125 148 49")
              {
                digitalWrite(charger,LOW);
                Serial.println("User1 Charging stopped");
                Serial.print("USER1 Remaining Balance: ");
                Serial.println(u1_bal);
                lcd.clear();
                lcd.print("User1 charging");  
                lcd.setCursor(0,1); 
                lcd.print("Stopped");
                delay(2000);
                lcd.clear();
                lcd.print("User1 Remaining");  
                lcd.setCursor(0,1); 
                lcd.print("Balance: ");
                lcd.print(u1_bal);
                delay(2000);
                break;
              }  
            }
            rfid.halt();
          }
          Serial.print("USER1 Balance: ");
          Serial.println(u1_bal);
          lcd.clear();
          lcd.print("User1 charging");  
          lcd.setCursor(0,1); 
          lcd.print("Rem balance: ");
          lcd.print(u1_bal);
          delay(1000);
        }
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(rfidCard=="16 213 43 28")
        {
          u1_bal=u1_bal+100;
          lcd.clear();
          lcd.print("User1 Recharged");  
          lcd.setCursor(0,1); 
          lcd.print("Rem balance: ");
          lcd.print(u1_bal);
          delay(2000);
        }
      }
    rfid.halt();
  }
  digitalWrite(charger,LOW);
  lcd.clear();
  lcd.print("Charge Your");
  lcd.setCursor(0,1); 
  lcd.print("Phone here");
  delay(500);
}
