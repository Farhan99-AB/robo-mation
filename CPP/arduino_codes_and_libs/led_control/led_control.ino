int ledpin = 3;
int ldrpin = 1;
int value = 0;

void setup()
{
                                                                                                                                                                                                                                                                               Serial.begin(9600);
}
void loop()
{
  value = analogRead(ldrpin);
  value = constrain(value,900,1010);
  value = map(value,900,1010,255,0);


  Serial.println(value);
  analogWrite(ledpin, value);

  delay(30);
}

