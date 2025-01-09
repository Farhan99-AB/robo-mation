// ZONE TRANSMITTER

const int sw1 = 8;            // switch to close
const int sw2 = 9;          // switch to open
const int sw3 = 10;            // switch to close



void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);

}

void loop()
{
  readSwitch();
  delay(500);
}


void readSwitch()
{
  
    Serial.print("A");
    delay(500);
 
}

