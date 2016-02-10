#include <OneWire.h>
#include <DallasTemperature.h>
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//define some variables
int deviceCount; //this is for a device count
 
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
}
 
 
void loop(void)
{
  deviceCount = sensors.getDeviceCount(); //get a count all all ds18b20s
  double temps[deviceCount]; //define an array to store temperatures
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
    
    /*
    Serial.print("Device "); 
    Serial.print(ii);
    Serial.print(" is ");
    Serial.print(sensors.getTempCByIndex(ii));
    Serial.print("\n");
    */
    
  }  // end of for 
  tmean = tmean / deviceCount;
  Serial.print("N,min,mean,max\n");
  Serial.print(deviceCount); Serial.print(" , ");
  Serial.print(tmin); Serial.print(" , ");
  Serial.print(tmean); Serial.print(" , ");
  Serial.print(tmax); Serial.print("\n");
 
}
/*

#include <OneWire.h> 
#include <DallasTemperature.h> 

// Data wire is plugged into these pins on the Arduino 
#define TEMP 10 

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs) 
OneWire oneWire1(TEMP); 

// Pass our oneWire reference to Dallas Temperature.  
DallasTemperature sensors1(&oneWire1); 

void setup(void) 
{   
// start serial port 
Serial.begin(9600); 
Serial.println("Sean's Program"); 

// Start up the library 
sensors1.begin();

} 
 
void loop(void) 
{    
// call sensors.requestTemperatures() to issue a global temperature  
// request to all devices on the bus 
Serial.println("Requesting temperatures..."); 
sensors1.requestTemperatures(); // Send the command to get temperatures from sensor1 

Serial.println(""); // Creates a line of space

// Now we run through every temperature sensor found on the 1wire bus on pin 10
Serial.print("A = "); 
Serial.println(sensors1.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire 
Serial.print("B = "); 
Serial.println(sensors1.getTempCByIndex(1));
Serial.print("C = "); 
Serial.println(sensors1.getTempCByIndex(2));
Serial.print("D = "); 
Serial.println(sensors1.getTempCByIndex(3));
Serial.print("E = "); 
Serial.println(sensors1.getTempCByIndex(4));
Serial.print("F = "); 
Serial.println(sensors1.getTempCByIndex(5));
Serial.print("G = "); 
Serial.println(sensors1.getTempCByIndex(6));
Serial.print("H = "); 
Serial.println(sensors1.getTempCByIndex(7));
Serial.print("I = "); 
Serial.println(sensors1.getTempCByIndex(8));
Serial.print("J = "); 
Serial.println(sensors1.getTempCByIndex(9));
Serial.print("K = "); 
Serial.println(sensors1.getTempCByIndex(10));

Serial.println(""); //creates a line of space
Serial.println(""); //creates a line of space

}*/
