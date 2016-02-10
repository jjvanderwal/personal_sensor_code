#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 // Data wire is plugged into pin 2 on the Arduino

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

int deviceCount; //this is for a device count
 
void setup(void) //needed to start with arduino startup
{
  Serial.begin(9600); // start serial port
  sensors.begin(); // Start up the library
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
  for (int i = 0; i < deviceCount; i++) //cycle through each of the sensors
  {
    sensors.getAddress(addr,i);
    Serial.print(batoh(addr));
    Serial.print(",");
    Serial.print(sensors.getTempC(addr));
    Serial.print("\n");
  }
}
 
void loop(void)
{
  Serial.println("BEGIN TEMPERATURE");
  log_temperature();
  Serial.println("  END TEMPERATURE");
}


