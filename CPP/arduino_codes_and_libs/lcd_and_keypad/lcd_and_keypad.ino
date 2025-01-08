// Include Arduino Wire library for I2C
#include <Wire.h> 
// Include LCD display library for I2C
#include <LiquidCrystal_I2C.h>
// Include Keypad library
#include <Keypad.h>

// Constants for row and column sizes
const byte ROWS = 4;
const byte COLS = 4;

// Array to represent keys on keypad
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Connections to Arduino
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

char data[6];
int data_count = 0;

// Create keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Create LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup(){
  // Setup LCD with backlight and initialize
  Serial.begin(9600);
  lcd.backlight();
  lcd.init(); 
  lcd.print("ONN");
  delay(1000);
  //lcd.clear();
}

void loop()
{
  weight();
}

void weight()
{
  // Get key value if pressed
  char customKey = customKeypad.getKey();
  
  if (customKey){
    // Clear LCD display and print character
    data[data_count] = customKey;
    lcd.print(data[data_count]);
    Serial.println(data[data_count]);
    data_count++;
  }
  if(customKey == '#')
  {
    Serial.print("Weight = ");
    print_data();
    Serial.println("gms");
    clear_data();
  }
}

void clear_data()
{
  while (data_count != 0) 
    {
      data[data_count--] = 0;
    }
}

void print_data()
{
  int i;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight= ");
  lcd.setCursor(8,0);
  for(i=0; i<data_count-1; i+=1)
  {
    Serial.print(data[i]);
    lcd.print(data[i]);
  }
  int n = 8+i;
  lcd.setCursor(n,0);
  lcd.print(" gms");
  delay(2000);
  lcd.clear();

}
