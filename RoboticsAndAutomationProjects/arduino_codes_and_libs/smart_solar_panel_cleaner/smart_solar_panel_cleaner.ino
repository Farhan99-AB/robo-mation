// Include Software Serial library to communicate with GSM
#include <SoftwareSerial.h>

// Configure software serial port
SoftwareSerial SIM900(7, 8);

// Variable to store text message
String textMessage;

// Create a variable to store Lamp state
String lampState = "HIGH";

int flag;

//limit switch 
int f_sw = A0;
int b_sw = A1;
int f_state;
int b_state;

//motor driver
const int enB = 10;
const int enA = 11;
const int lf_motor = 3;
const int lb_motor = 4;
const int rf_motor = 5;
const int rb_motor = 6;

//cleaner
int clean = 12;

int pre_state;

void setup() 
{
  // Automatically turn on the shield
  //digitalWrite(9,HIGH);
  //delay(1000);
  //digitalWrite(9, LOW);
  //delay(5000);
  
  // Set motor as OUTPUT
  pinMode(lf_motor, OUTPUT);
  pinMode(lb_motor, OUTPUT);
  pinMode(rf_motor, OUTPUT);
  pinMode(rb_motor, OUTPUT);

  //cleaner
  pinMode(clean, OUTPUT);

  //limit switch
  pinMode(f_sw, INPUT_PULLUP);
  pinMode(b_sw, INPUT_PULLUP);

  // By default the motor is off
  digitalWrite(lf_motor, LOW);
  digitalWrite(lb_motor, LOW);
  digitalWrite(rf_motor, LOW);
  digitalWrite(rb_motor, LOW);

  //cleaner off
  digitalWrite(clean,LOW);
  
  // Initializing serial commmunication
  Serial.begin(19200); 
  SIM900.begin(19200);
  // Give time to your GSM shield log on to network
  delay(20000);

  limit_switch();
  if(f_state > 500 && b_state > 500)
  {
    Serial.println("Limit Switch off");
    // AT command to set SIM900 to SMS mode
    SIM900.print("AT+CMGF=1\r"); 
    delay(100);
    // Set module to send SMS data to serial out upon receipt 
    SIM900.print("AT+CNMI=2,2,0,0,0\r");
    delay(100);
  
    String message = "Limit Switch off";
    sendSMS(message);
  }
  
  while(1)
  {
    limit_switch();
    if(f_state < 500 || b_state < 500)
    {
      Serial.print("SIM900 ready...");
    
      // AT command to set SIM900 to SMS mode
      SIM900.print("AT+CMGF=1\r"); 
      delay(100);
      // Set module to send SMS data to serial out upon receipt 
      SIM900.print("AT+CNMI=2,2,0,0,0\r");
      delay(100);
    
      String message = "Cleaner Ready";
      sendSMS(message);
      break;
    }
  }
}

void loop()
{
  limit_switch();
  message_read();
  if(textMessage.indexOf("ON")>=0)
  {
    send_motor_on(); 
  }
  else if(textMessage.indexOf("OFF")>=0)
  {
    send_motor_off(); 
  }
  else if(textMessage.indexOf("STATE")>=0)
  {
    send_motor_state();
  } 
  motor();
}

void message_read()
{
  if(SIM900.available()>0)
  {
    textMessage = SIM900.readString();
    Serial.print(textMessage);    
    delay(10);
  }
}

void send_motor_on()
{
    // Turn on motor and save current state
    motor_move();
    digitalWrite(clean, HIGH);
    lampState = "on";
    String message = "Cleaner turned " + lampState;
    sendSMS(message);
    Serial.println("Relay set to ON");  
    textMessage = "";   
}

void send_motor_off()
{
    // Turn off motor and save current state
    flag = 1;
    motor_stop();
    digitalWrite(clean, LOW);
    lampState = "off";
    String message = "Cleaner turned " + lampState;
    sendSMS(message); 
    Serial.println("Relay set to OFF");
    textMessage = ""; 
}

void send_motor_state()
{
    String message = "Cleaner is " + lampState;
    sendSMS(message);
    Serial.println("Lamp state resquest");
    textMessage = "";
}

// Function that sends SMS
void sendSMS(String message)
{
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  SIM900.println("AT + CMGS = \"+919686388986\""); 
  delay(100);
  // Send the SMS
  SIM900.println(message); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000);  
}

void limit_switch()
{
  f_state = analogRead(f_sw);
  b_state = analogRead(b_sw);
}

void motor_move()
{
  flag = 0;
  if(f_state > 500 && b_state < 500) // moving forward
  {
    pre_state = 1;//was moving forward
    analogWrite(enA, 120);
    analogWrite(enB, 120);
    digitalWrite(lf_motor, HIGH);
    digitalWrite(lb_motor, LOW);
    digitalWrite(rf_motor, HIGH);
    digitalWrite(rb_motor, LOW);
  }
  else if(f_state < 500 && b_state > 500) // moving backward
  {
    pre_state = 0;//was moving backward
    analogWrite(enA, 80);
    analogWrite(enB, 80);
    digitalWrite(lf_motor, LOW);
    digitalWrite(lb_motor, HIGH);
    digitalWrite(rf_motor, LOW);
    digitalWrite(rb_motor, HIGH);
  }
}

void motor_stop()
{
  digitalWrite(lf_motor, LOW);
  digitalWrite(lb_motor, LOW);
  digitalWrite(rf_motor, LOW);
  digitalWrite(rb_motor, LOW);
}

void motor()
{
  if(flag != 1)
  {
    if(pre_state == 1 && f_state < 500 && b_state > 500)
    {
      send_motor_off();
    }
    else if(pre_state == 0 && f_state > 500 && b_state < 500)
    {
      send_motor_off();
    }
  }
}
