// Do not remove the include below
#include "Ecofest.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS2438a.h>
#include "DS3231.h"
#include <avr/power.h>
#include <avr/sleep.h>

#define ONE_WIRE_BUS_01 9       //Temp -     Brown
#define ONE_WIRE_BUS_02 11 //Supposed to be 30      //Humidity - Orange
#define ONE_WIRE_BUS_03 12

//#define XBEE_POWER_PIN 5

//OneWire Busses
OneWire  owb1(ONE_WIRE_BUS_01);
OneWire  owb2(ONE_WIRE_BUS_02);
OneWire  owb3(ONE_WIRE_BUS_03);

bool __DEBUG__ = false;
long SLEEP_ERROR_MARGIN = 20000;
long sleep_seconds = 60;

DateTime wake_time;

//DS3231 RTC;
DS3231 RTC;

//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(57600);

	//Setup RTC
	Wire.begin();
	RTC.begin();
	RTC.clearINTStatus();
	wake_time = RTC.now();
//	pinMode(XBEE_POWER_PIN, OUTPUT);
//	digitalWrite(XBEE_POWER_PIN, HIGH);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) {
    	Serial.print("0");
    }
    Serial.print(deviceAddress[i], HEX);
  }
}

void log_temperature(OneWire &owb){
//	Serial.println("Getting Temperature Readings...");
	DeviceAddress temp_addr;
	DallasTemperature dallasTemp(&owb);

	dallasTemp.begin();
	int numberOfDevices_01 = dallasTemp.getDeviceCount();
	dallasTemp.requestTemperatures();
	DateTime now = RTC.now();
	for(int i=0;i<numberOfDevices_01; i++)
	{
		// Search the wire for address
		if(dallasTemp.getAddress(temp_addr, i) &&  temp_addr[0] == 0x28)
		{
			Serial.print(now.get());
			Serial.print(",");
			printAddress(temp_addr);
			Serial.print(",");
			Serial.println(dallasTemp.getTempC(temp_addr), DEC);
		}
	}
}

void log_humidity(OneWire &owb){
	DeviceAddress hum_addr;
	DS2438a hum(&owb, hum_addr);

	owb2.reset_search();
	while(owb2.search(hum_addr))
	{

		if(hum_addr[0] == 0x26){
			DateTime now = RTC.now();
			Serial.print(now.get());
			Serial.print(",");
			printAddress(hum_addr);
			Serial.print(",");
			Serial.print(hum.readTempC(), DEC);
			Serial.print(",");
			Serial.print(hum.readSupplyVoltage(), DEC);
			Serial.print(",");
			Serial.println(hum.readSensorVoltage(), DEC);

		}
	}
}

#define sleep_bod_disable() \
{ \
  uint8_t tempreg; \
  __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" \
                       "ori %[tempreg], %[bods_bodse]" "\n\t" \
                       "out %[mcucr], %[tempreg]" "\n\t" \
                       "andi %[tempreg], %[not_bodse]" "\n\t" \
                       "out %[mcucr], %[tempreg]" \
                       : [tempreg] "=&d" (tempreg) \
                       : [mcucr] "I" _SFR_IO_ADDR(MCUCR), \
                         [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                         [not_bodse] "i" (~_BV(BODSE))); \
}

void INT0_ISR()
{
  //Keep this as short as possible. Possibly avoid using function calls
    detachInterrupt(0);

}

void init_sleep(void){
     PORTD |= 0x04;
     DDRD &=~ 0x04;


     attachInterrupt(0, INT0_ISR, LOW); //Only LOW level interrupt can wake up from PWR_DOWN
     set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}



void power_down(int numberOfDevices)
{
     if(__DEBUG__){
       Serial.print("$RTC: ");
       Serial.print(RTC.now().get());
       Serial.print(", To Wake: ");
       Serial.print(wake_time.get());
       Serial.print(", Difference: ");
       Serial.println(wake_time.get() - RTC.now().get());
     }

    init_sleep();
    RTC.clearINTStatus(); //This function call is  a must to bring /INT pin HIGH after an interrupt.

    Serial.flush(); //Ensure all data is sent before we sleep.

    //Ensure that we set our wake up time in the future.
    if (wake_time.get() - RTC.now().get() < SLEEP_ERROR_MARGIN)
    {
       wake_time = DateTime(wake_time.get() + sleep_seconds);
    }
    RTC.enableInterrupts(wake_time.hour(), wake_time.minute(), wake_time.second());    // set the interrupt at (h,m,s)
    attachInterrupt(0, INT0_ISR, LOW);  //Enable INT0 interrupt (as ISR disables interrupt). This strategy is required to handle LEVEL triggered interrupt

    //Power Down routines
    cli();
    sleep_enable();      // Set sleep enable bit
    sleep_bod_disable(); // Disable brown out detection during sleep. Saves more power
    sei();

//    delay(10+(5*numberOfDevices)); //This delay is required to allow print to complete
//    digitalWrite(LED_PIN, LOW);
//    LED_STATE = LOW;
    //Shut down all peripherals like ADC before sleep. Refer Atmega328 manual
    power_all_disable(); //This shuts down ADC, TWI, SPI, Timers and USART
    sleep_cpu();         // Sleep the CPU as per the mode set earlier(power down)
    sleep_disable();     // Wakes up sleep and clears enable bit. Before this ISR would have executed
    power_all_enable();  //This shuts enables ADC, TWI, SPI, Timers and USART

    delay(50); //This delay is required to allow CPU to stabilize
//    digitalWrite(LED_PIN, HIGH);
//    LED_STATE = HIGH;

    if(__DEBUG__){
      Serial.print("$RTC: ");
      Serial.print(RTC.now().get());
      Serial.print(", Wake: ");
      Serial.print(wake_time.get());
      Serial.print(", Sleep: ");
      wake_time = DateTime(wake_time.get() + sleep_seconds);
      Serial.println(wake_time.get());
    }
    wake_time = RTC.now();
//    Serial.println("Awake from sleep");
}

// The loop function is called in an endless loop
void loop()
{
	Serial.println("BEGIN TEMPERATURE");
	log_temperature(owb1);
	log_temperature(owb3);
	Serial.println("  END TEMPERATURE");
	Serial.println("BEGIN HUMIDITY");
	log_humidity(owb2);
	Serial.println("  END HUMIDITY");
	power_down(100);

}


