#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
 
#define ONE_WIRE_BUS 2 // Data wire is plugged into pin 2 on the Arduino
 
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
int deviceCount; //this is for a device count

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // select the pins used on the LCD panel
 
void setup(void)
{
  sensors.begin(); // Start up the onewire library
  lcd.begin(16, 2);  // start the lcd library
  
  lcd.setCursor(0,0);
  lcd.print("N min mean max"); // print a simple message
  
}
 
 
void loop(void)
{
  deviceCount = sensors.getDeviceCount(); //get a count all all ds18b20s
  double tmin, tmean, tmax;
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.print(sensors.getTempCByIndex(0));
  for (int ii = 0; ii < deviceCount; ii++)
  {
    if (ii==0)
    {
      tmin = tmean = tmax = sensors.getTempCByIndex(ii);
    }
    else
    {
      tmean = tmean + sensors.getTempCByIndex(ii);
      if (sensors.getTempCByIndex(ii)<tmin) tmin = sensors.getTempCByIndex(ii);
      if (sensors.getTempCByIndex(ii)>tmax) tmax = sensors.getTempCByIndex(ii);
    }
  }  // end of for 
  tmean = tmean / deviceCount;
  
  lcd.setCursor(0,1);
  lcd.print(deviceCount); // print a simple message
  lcd.setCursor(3,1);
  lcd.print(tmin); // print a simple message
  lcd.setCursor(8,1);
  lcd.print(tmean); // print a simple message
  lcd.setCursor(13,1);
  lcd.print(tmax); // print a simple message
  
 
}

