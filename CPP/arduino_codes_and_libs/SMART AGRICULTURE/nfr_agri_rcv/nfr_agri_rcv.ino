#include <SoftwareSerial.h>

int led_pin = 2;
int button_state = 0;

SoftwareSerial HC12(9, 10); // HC-12 TX Pin, HC-12 RX Pin

void setup() 
{
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  Serial.println("Radio ONN");

}

void loop() 
{
  while (HC12.available()) 
  {        // If HC-12 has data
    //Serial.write(HC12.read());      // Send the data to Serial monitor
    int text = HC12.read();
    if(text == 1)
    {
      Serial.println("HIGH");
      digitalWrite(led_pin, HIGH);
      Serial.println(text);
    }
    else if(text == 0)
    {
      Serial.println("LOW");
      digitalWrite(led_pin, LOW);
      Serial.println(text);
    }
  }
//  while (Serial.available()) 
//  {      // If Serial monitor has data
//    HC12.write(Serial.read());      // Send that data to HC-12
//  }
}
