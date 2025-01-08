#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7) 

#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);

String rfidCard;

#define charger 8
#define rechargebtn 1

int u1_bal = 100;
int u2_bal = 100;

long int stime=0;


void setup() 
{
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();

  pinMode(charger,OUTPUT);
  digitalWrite(charger,LOW);

  pinMode(recharge,INPUT_PULLUP);

  lcd.begin(16,2);

  lcd.print("RFID CHARGING"); 
  delay(3000); 
  lcd.setCursor(0,1); 
  lcd.print("Machine ON");
  delay(1000);  
}

void loop() 
{
  if(digitalRead(rechargebtn)==0)
  {
    recharge();
  }
  if (rfid.isCard()) 
  {
    if (rfid.readCardSerial()) 
    {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);

      ///////////////////////////////////////////////////////////////////////////////USER1
      if(rfidCard=="3 2 206 5")
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
              if(rfidCard=="3 2 206 5")
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
      }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      ///////////////////////////////////////////////////////////////////////////////USER2
      if(rfidCard=="17 237 90 46")
      {
        digitalWrite(charger,HIGH);
        Serial.println("USER2 charging");
        lcd.clear();
        lcd.print("User2 charging");  
        lcd.setCursor(0,1); 
        lcd.print("Rem balance: ");
        lcd.print(u2_bal);
        while(u2_bal>0)
        {
          u2_bal=u2_bal-1;
          if (rfid.isCard()) 
          {
            if (rfid.readCardSerial()) 
            {
              rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
              if(rfidCard=="17 237 90 46")
              {
                digitalWrite(charger,LOW);
                Serial.println("User2 Charging stopped");
                Serial.print("USER2 Remaining Balance: ");
                Serial.println(u2_bal);
                lcd.clear();
                lcd.print("User2 charging");  
                lcd.setCursor(0,1); 
                lcd.print("Stopped");
                delay(2000);
                lcd.clear();
                lcd.print("User2 Remaining");  
                lcd.setCursor(0,1); 
                lcd.print("Balance: ");
                lcd.print(u2_bal);
                delay(2000);
                break;
              }  
            }
            rfid.halt();
            }
            Serial.print("USER2 Balance: ");
            Serial.println(u1_bal);
            lcd.clear();
            lcd.print("User2 charging");  
            lcd.setCursor(0,1); 
            lcd.print("Rem balance: ");
            lcd.print(u2_bal);
            delay(1000);
        }
      }/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void recharge()
{
  while(1)
  {
    lcd.clear();
    lcd.print("Tap card to");  
    lcd.setCursor(0,1); 
    lcd.print("Recharge");
    if (rfid.isCard()) 
    {
      if (rfid.readCardSerial()) 
      {
        rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
        if(rfidCard=="3 2 206 5")
        {
          u1_bal=u1_bal+100;
          lcd.clear();
          lcd.print("User1 Recharged");  
          lcd.setCursor(0,1); 
          lcd.print("Rem balance: ");
          lcd.print(u1_bal);
          delay(2000);
          break;
        }
        if(rfidCard=="17 237 90 46")
        {
          u2_bal=u2_bal+100;
          lcd.clear();
          lcd.print("User2 Recharged");  
          lcd.setCursor(0,1); 
          lcd.print("Rem balance: ");
          lcd.print(u2_bal);
          delay(2000);
          break;
        }
      }
      rfid.halt();
    }
  }
}
