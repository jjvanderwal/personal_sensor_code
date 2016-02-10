//////////////////////////////////////////////////////////////////
//©2013 drafted by Jeremy VanDerWal
//initial control of temperature of a peltier with temperature
//////////////////////////////////////////////////////////////////
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
 
#define fadePin 3 //pin to control mosfet
#define tempPin 2 //one-wire pin to read temperature

OneWire oneWire(tempPin); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // select the pins used on the LCD panel

float target_temp = 20.00; //define the target temp
float cur_temp; // variable for the current temperature

void setup(){
  pinMode(fadePin, OUTPUT); //set the pinmode for the mosfet
  sensors.begin(); // Start up the library
  lcd.begin(16, 2);  // start the lcd library
  lcd.setCursor(0,0); // set cursor position
  lcd.print("pelt temp:"); // print a simple message
  analogWrite(fadePin, 255); //turn on the peltier
}

void loop(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  cur_temp = sensors.getTempCByIndex(0);
  lcd.setCursor(11,0); // set cursor position
  lcd.print(cur_temp); // print the temperature
  if (cur_temp < target_temp) { 
    analogWrite(fadePin, 0);  //turn off the peltier
  } else {
    analogWrite(fadePin, 255); //turn on the peltier
  }
  

}

