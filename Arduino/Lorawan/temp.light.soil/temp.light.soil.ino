/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * initial work is with Wemos D1 mini and then will work with Lorawan 
  * altered to now work with Seeeduino Stalker 2.3 & this is working with Lora
  * here we want temperature and soil moisture reported.
  * GNU General Public License .. feel free to use / distribute ... no warranties
  * 
  * soil = A1
  * light = A3
  * DS18B20 = D2
 */

#include <OneWire.h>              //this is the onewire library
#include <DallasTemperature.h>    //include the dallas temperature library
#include <LoRaAT.h>               //Include LoRa AT libraray
#include <SoftwareSerial.h>       //Software serial for debug
#include <Battery.h>              //include the battery library

//define and initialize some of the pins/types 
#define ONE_WIRE_BUS 4                 // Data wire is plugged into pin 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);          // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature.
SoftwareSerial debugSerial(10, 11);     // RX, TX
LoRaAT mdot(0, &debugSerial);           //Instantiate a LoRaAT object
Battery battery;                        //initialize the battery

// Variable Setup
int deviceCount;                        //this is for a device count
unsigned long updateInterval = 54000;   //set the update interval to 60 seconds
int responseCode;                       //define the responsecode for joining the lora network
  
// setup the start
void setup() {
  sensors.begin();                      //start the ds18b20 temperature sensors                                                
  
  debugSerial.begin(38400);             //Debug output. Listen on this ports for debugging info
  mdot.begin(38400);                    //Begin (possibly amongst other things) opens serial comms with MDOT
   
  do {                                  //join the lora network
    responseCode = mdot.join();
    delay(10000);
  } while (responseCode != 0);
 
}

//the looping script
void loop() {
  
  String postData = ("");                                           //define the initial post data
  
  deviceCount = sensors.getDeviceCount();                           //get a count all all ds18b20s
  sensors.requestTemperatures();                                    // Send the command to get temperatures
  String Temp1 = String(sensors.getTempCByIndex(0));                //get all the temperature values
  postData += ("field1:" + Temp1);                                  //keep only the first temperature value for now

  String Soil1 = String(analogRead(A1));                            //read the analog soil value
  postData += (",field2:" + Soil1);                                 //append it to the post data

  String light1 = String(analogRead(A3));                            //read the analog ligh value
  postData += (",field3:" + light1);                                 //append it to the post data

  battery.update();                                                  //get the battery information
  String voltage = String(battery.getVoltage());                     //get the voltage 
  String percentage = String(battery.getPercentage());               //what is the charge percentage
  postData += (",field7:" + voltage + ",field8:" + percentage);      //append it to the post data

  debugSerial.println(postData);
  
  char postDataChar[100];
  postData.toCharArray(postDataChar,99);
  responseCode = mdot.sendPairs(postDataChar);

  debugSerial.println("posted");
      
  delay(updateInterval); // delay to let things reset

  debugSerial.println("delay done");
}

