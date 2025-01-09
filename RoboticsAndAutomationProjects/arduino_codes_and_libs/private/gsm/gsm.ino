#include <SoftwareSerial.h>

SoftwareSerial mySerial(9, 10);
#define limit_s A1    
   
void setup()
    {
     pinMode(limit_s,INPUT_PULLUP);
      Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
      delay(100);
      mySerial.begin(9600);   // Setting the baud rate of GSM Module  
     
    }

void loop()

  {
    int limit_state=analogRead(limit_s);
    Serial.println(limit_state);  
  if(analogRead(limit_state)<500)
  {
      SendMessage();
      RecieveMessage();
  }  
 if (mySerial.available()>0)
   Serial.write(mySerial.read());
}


 void SendMessage()
{
  mySerial.println("AT+CMGF=1\r");    //Sets the GSM Module in Text Mode
  delay(5000);  // Delay of 1000 milli seconds or 1 second
  Serial.println("Gsm Into SMS mode");
  mySerial.println("AT+CMGS=\"+918951450245\""); // Replace x with mobile number
  delay(5000);
  mySerial.println("DANGER save me");// The SMS text you want to send
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(5000);
  Serial.println("Message sent");
  delay(5000);
}


 void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
