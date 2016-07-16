/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * initial work is with Wemos D1 mini and then will work with Lorawan 
  * altered to now work with Seeeduino Stalker 2.3 & this is working with Lora
  * here we want temperature and soil moisture reported.
  * GNU General Public License .. feel free to use / distribute ... no warranties
  * 
  * soil = A1
  * light = A3
  * DS18B20 = D4
 */

#include <OneWire.h>              //this is the onewire library
#include <DallasTemperature.h>    //include the dallas temperature library
#include <LoRaAT.h>               //Include LoRa AT libraray
#include <SoftwareSerial.h>       //Software serial for debug
#include <avr/sleep.h>                  // this is for low power sleep
#include <avr/power.h>                  // this is for low power sleep
#include <Wire.h>                       // ??? have not looked up library yet but was in example
#include "DS3231.h"                     // RTC
#include <math.h>

//define and initialize some of the pins/types 
#define ONE_WIRE_BUS 4                  // Data wire is plugged into pin 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);          // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature.
SoftwareSerial debugSerial(10, 11);     // RX, TX
LoRaAT mdot(0, &debugSerial);           //Instantiate a LoRaAT object
int POWER_BEE = 5;                      // power_bee pin is 5

// Variable Setup
int deviceCount;                        //this is for a device count
int responseCode;                       //define the responsecode for joining the lora network
DS3231 RTC;                             // Create the DS3231 RTC interface object
static DateTime interruptTime;          // this is the time to interupt sleep
int numtries;                           // define number of tries to connect

// setup the start
void setup() {
  sensors.begin();                      //start the ds18b20 temperature sensors                                                
  
  debugSerial.begin(38400);             //Debug output. Listen on this ports for debugging info
  mdot.begin(38400);                    //Begin (possibly amongst other things) opens serial comms with MDOT

  pinMode(POWER_BEE, OUTPUT);

  /*Initialize INT0 for accepting interrupts */
  PORTD |= 0x04; 
  DDRD &=~ 0x04;
  pinMode(4,INPUT);                     //extern power
  
  Wire.begin();    
  RTC.begin();
  attachInterrupt(0, INT0_ISR, LOW); 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  DateTime start = RTC.now();
  interruptTime = DateTime(start.get() + 300); //Add 5 mins in seconds to start time

}

//start the application
void loop () 
{
  ///////////////// START the application ////////////////////////////
  
  String postData = ("");                                           //define the initial post data
  
  deviceCount = sensors.getDeviceCount();                           //get a count all all ds18b20s
  sensors.requestTemperatures();                                    // Send the command to get temperatures
  String Temp1 = String(sensors.getTempCByIndex(0));                //get all the temperature values
  postData += ("T1:" + Temp1);                                  //keep only the first temperature value for now

  String Soil1 = String(analogRead(A1));                            //read the analog soil value
  postData += (",S2:" + Soil1);                                 //append it to the post data

  String light1 = String(analogRead(A3));                            //read the analog ligh value
  postData += (",L3:" + light1);                                 //append it to the post data

  String CHstatus = String(read_charge_status());                    //read the charge status
  int BatteryValue = analogRead(A7);                                 // read the battery voltage
  float voltage = BatteryValue * (1.1 / 1024)* (10+2)/2;             //Voltage devider
  String Volts = String(round(voltage*100)/100);                     //get the voltage 
  postData += (",V:" + Volts + ",CH:" + CHstatus);      //append it to the post data

  //Serial.println(postData);
  debugSerial.println(postData);

  PostData(postData);
  
  RTC.clearINTStatus(); //This function call is  a must to bring /INT pin HIGH after an interrupt.
  RTC.enableInterrupts(interruptTime.hour(),interruptTime.minute(),interruptTime.second());    // set the interrupt at (h,m,s)
  attachInterrupt(0, INT0_ISR, LOW);  //Enable INT0 interrupt (as ISR disables interrupt). This strategy is required to handle LEVEL triggered interrupt

  ////////////////// Application finished... put to sleep ///////////////////
  
  //\/\/\/\/\/\/\/\/\/\/\/\/Sleep Mode and Power Down routines\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
        
  //Power Down routines
  cli(); 
  sleep_enable();      // Set sleep enable bit
  sleep_bod_disable(); // Disable brown out detection during sleep. Saves more power
  sei();
    
  debugSerial.println("\nSleeping");
  delay(10); //This delay is required to allow print to complete
  //Shut down all peripherals like ADC before sleep. Refer Atmega328 manual
  power_all_disable(); //This shuts down ADC, TWI, SPI, Timers and USART
  sleep_cpu();         // Sleep the CPU as per the mode set earlier(power down) 
   
  sleep_disable();     // Wakes up sleep and clears enable bit. Before this ISR would have executed
  power_all_enable();  //This shuts enables ADC, TWI, SPI, Timers and USART
  delay(10); //This delay is required to allow CPU to stabilize
  debugSerial.println("Awake from sleep");    
  
  //\/\/\/\/\/\/\/\/\/\/\/\/Sleep Mode and Power Saver routines\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
 
} 

//Interrupt service routine for external interrupt on INT0 pin conntected to DS3231 /INT
void INT0_ISR()
{
  //Keep this as short as possible. Possibly avoid using function calls
    detachInterrupt(0); 
    interruptTime = DateTime(interruptTime.get() + 300);  //decide the time for next interrupt, configure next interrupt  
}




//this is the setup to post data
void PostData(String str2post) {
  digitalWrite(5, HIGH);        //turn the xbee port on
  delay(1000);
  numtries = 0;                         //set number of tries to 0 as a counter
  do {                                  //join the lora network
    responseCode = mdot.join();         //join the network and get the response code
    delay(10000);
    numtries += 1;                      //add to number of tries
  } while (responseCode != 0);          //continue if it joins

  //TODO exit if numtries >10
  
  char postDataChar[100];
  str2post.toCharArray(postDataChar,99);
  responseCode = mdot.sendPairs(postDataChar);

  debugSerial.println("posted");

  
  digitalWrite(POWER_BEE, LOW);         //turn the xbee port off
}

//get the charging status
unsigned char read_charge_status(void) {
  unsigned char CH_Status=0;
  unsigned int ADC6=analogRead(6);
  if(ADC6>900) {
    CH_Status = 0;//sleeping
  } else if(ADC6>550) {
    CH_Status = 1;//charging
  } else if(ADC6>350) {
    CH_Status = 2;//done
  } else {
    CH_Status = 3;//error
  }
  return CH_Status;
}

