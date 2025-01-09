#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "FUTURE TECHNOLOGYS";       // AP NAME
String PASS = "9916143610";    //"9916143610"; // AP PASSWORD
String API = "X8P9C5PHRAX6JC03";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
String field5 = "field5";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor1=1;
int valSensor2=1;
int valSensor3=1;
int valSensor4=1;
int valSensor5=1;
SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() 
{
  valSensor1 = getSensorData();
  valSensor2 = getSensorData();
  valSensor3 = getSensorData();
  valSensor4 = getSensorData();
  valSensor5 = getSensorData();
 String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(0)+ "&"+ field2 +"="+String(0)+ "&"+ field3 +"="+String(0)+ "&"+ field4 +"="+String(0)+"&"+field5 +"="+String(0);
 Serial.println(getData);
 
 //String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 delay(15000);
}

int getSensorData(){
  return random(1000); // Replace with your own sensor code
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
