#define ldr1 A0
#define ldr2 A1
#define r1 2 
#define r2 3

int tol=40,v1,v2;

void setup() 
{
  Serial.begin(9600);
  pinMode(r1,OUTPUT);
  pinMode(r2,OUTPUT);
  pinMode(ldr1,INPUT_PULLUP);
  pinMode(ldr2,INPUT_PULLUP);
}

void loop() 
{
  solar_tracker();
}

void solar_tracker()
{
  v1=analogRead(ldr1);
  v2=analogRead(ldr2);

  Serial.print(v1);
  Serial.print(" ");
  Serial.print(v2);
  Serial.print(" ");
  
  if ((v1>v2)&&(abs(v1-v2)>tol))
  {
    digitalWrite(r1,HIGH);
    digitalWrite(r2,LOW);
    Serial.println("CCW");
  }
  else if ((v1<v2)&&(abs(v1-v2)>tol))
  {
    digitalWrite(r1,LOW);
    digitalWrite(r2,HIGH);
    Serial.println("CW");
  }
  else
  {
    digitalWrite(r1,LOW);
    digitalWrite(r2,LOW);
    Serial.println("");
  }
}
