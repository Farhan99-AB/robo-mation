// Include Software Serial library to communicate with GSM
#include <SoftwareSerial.h>

// Configure software serial port
SoftwareSerial SIM900(7, 8);

const int enB = 10;
const int enA = 11;
const int lf_motor = 3;
const int lb_motor = 4;
const int rf_motor = 5;
const int rb_motor = 6;

// Variable to store text message
String textMessage;

// Create a variable to store Lamp state
String lampState = "HIGH";

// Relay connected to pin 12
const int relay = 12;

void setup() {
  // Automatically turn on the shield
//  digitalWrite(9,HIGH);
//  delay(1000);
//  digitalWrite(9, LOW);
//  delay(5000);
  
  // Set relay as OUTPUT
  pinMode(relay, OUTPUT);

  // By default the relay is off
  digitalWrite(relay, LOW);

   // Set motor as OUTPUT
  pinMode(lf_motor, OUTPUT);
  pinMode(lb_motor, OUTPUT);
  pinMode(rf_motor, OUTPUT);
  pinMode(rb_motor, OUTPUT);
  
  // Initializing serial commmunication
  Serial.begin(19200); 
  SIM900.begin(19200);

  // Give time to your GSM shield log on to network
  delay(20000);
  Serial.print("SIM900 ready...");

  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);

  String message = "Cleaner Ready";
  sendSMS(message);
}

void loop(){
  if(SIM900.available()>0){
    textMessage = SIM900.readString();
    Serial.print(textMessage);    
    delay(10);
  } 
  if(textMessage.indexOf("ON")>=0){
    // Turn on relay and save current state
    digitalWrite(relay, HIGH);
    analogWrite(enA, 120);
    analogWrite(enB, 120);
    digitalWrite(lf_motor, HIGH);
    digitalWrite(lb_motor, LOW);
    digitalWrite(rf_motor, HIGH);
    digitalWrite(rb_motor, LOW);
    lampState = "on";
    String message = "Cleaner turned " + lampState;
    sendSMS(message);
    Serial.println("Relay set to ON");  
    textMessage = "";   
  }
  if(textMessage.indexOf("OFF")>=0){
    // Turn off relay and save current state
    digitalWrite(relay, LOW);
    digitalWrite(lf_motor, LOW);
    digitalWrite(lb_motor, LOW);
    digitalWrite(rf_motor, LOW);
    digitalWrite(rb_motor, LOW);
    lampState = "off";
    String message = "Cleaner turned " + lampState;
    sendSMS(message); 
    Serial.println("Relay set to OFF");
    textMessage = ""; 
  }
  if(textMessage.indexOf("STATE")>=0){
    String message = "Cleaner is " + lampState;
    sendSMS(message);
    Serial.println("Lamp state resquest");
    textMessage = "";
  }
}  

// Function that sends SMS
void sendSMS(String message){
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
