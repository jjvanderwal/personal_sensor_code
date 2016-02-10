#include <OneWire.h>
#include <DallasTemperature.h>
#include <Time.h>  
#include <Wire.h>  
#include <DS3232RTC.h>  // a basic DS3232 library that returns time as a time_t

#define ONE_WIRE_BUS 2 // Data wire is plugged into pin 2 on the Arduino

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

int deviceCount; //this is for a device count
 
void setup(void) //needed to start with arduino startup
{
  Serial.begin(57600); // start serial port
  sensors.begin(); // Start up the library
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  for (int ii = 8; ii < 13; ii++) { pinMode(ii, OUTPUT); } // initialize the digital pin as an output.
}

char* batoh(uint8_t deviceAddress[]) { //function to get the address in a user readable format
  char* buf_str = (char*) malloc(2*8 + 1);
  char* buf_ptr = buf_str;
  for (uint8_t i = 0; i < 8; i++) {
    buf_ptr += sprintf(buf_ptr, "%02X", deviceAddress[i]);
  }
  *(buf_ptr) = '\0';
  return buf_str;
} 

void log_temperature(){
  deviceCount = sensors.getDeviceCount(); //get a count all all ds18b20s
  uint8_t addr[8]; //setup to store the address of the sensor
  sensors.requestTemperatures(); // Send the command to get temperatures
  time_t t = now()-SECS_YR_2000; //get the current time
  for (int i = 0; i < deviceCount; i++) //cycle through each of the sensors
  {
    digitalWrite(i+8,HIGH);
    Serial.print(t);
    Serial.print(",");
    sensors.getAddress(addr,i);
    char* ad = batoh(addr);
    Serial.print(ad);
    free(ad);
    Serial.print(",");
    Serial.print(sensors.getTempC(addr));
    Serial.print("\n");
    delay(200);
    digitalWrite(i+8,LOW);
  }
}
 
void loop(void)
{
  Serial.println("BEGIN TEMPERATURE");
  log_temperature();
  Serial.println("  END TEMPERATURE");
  Serial.println("BEGIN HUMIDITY");
  Serial.println("  END HUMIDITY");
  //for (int ii = 8; ii < 13; ii++) { digitalWrite(ii,LOW); } // initialize the digital pin as an output.
}


