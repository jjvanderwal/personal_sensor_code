//////////////////////////////////////////////////////////////////
//©2013 drafted by Jeremy VanDerWal
//initial control of temperature of a peltier with temperature
//////////////////////////////////////////////////////////////////
#include <OneWire.h>
#include <DallasTemperature.h>

#define fadePin 3 //pin to control mosfet
#define tempPin 2 //one-wire pin to read temperature

OneWire oneWire(tempPin); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

void setup(){
  pinMode(fadePin, OUTPUT); //set the pinmode for the mosfet
  Serial.begin(9600); //start a serial port
  sensors.begin(); // Start up the library
}

void loop(){
  analogWrite(fadePin, 255);
  delay(10000);
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("Temperature for Device 1 is: ");
  Serial.print(sensors.getTempCByIndex(0));
  analogWrite(fadePin, 0);
  delay(10000);
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("Temperature for Device 1 is: ");
  Serial.print(sensors.getTempCByIndex(0));
  
}

