/*
 This is a test program writen for the Seeeduino Stalker v2.3 and 
 uses the Multitech mDOT LoRa module running the Australian compatable AT
 enabled firmware.
 
 This program,
  * Joins the LoRa Network.
  * Sends predicitable data (a loop count), and temperature  (The DS3231
 does a temperature conversion once every 64 seconds. This is also the
 default for the DS3232.)
*/

/*--------------------------------------------------------------------------------------
  Includes
  --------------------------------------------------------------------------------------*/
#include <SoftwareSerial.h>
/*--------------------------------------------------------------------------------------
  Definitions
  --------------------------------------------------------------------------------------*/

SoftwareSerial debugSerial(10, 11);     // RX, TX

/*--- setup() --------------------------------------------------------------------------
  Called by the Arduino framework once, before the main loop begins.

  In the setup() routine:
   - Opens serial communication with MDOT
  --------------------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  debugSerial.begin(38400);   //Debug output. Listen on this ports for debugging info
  debugSerial.println("\r\n\r\n++ START ++\r\n\r\n");
  Serial.println("AT+JOIN");
  delay(10000);
  if(Serial.available() > 0) {
    debugSerial.println("Data received");
    debugSerial.write(Serial.read()); 
   }
  Serial.println("AT+SEND {\"Temp\":1");
  debugSerial.println("1st packet sent");
  delay(10000);
  Serial.println("AT+SEND ,\" RH\":2}");
  debugSerial.println("2nd packet sent");
  delay(10000);
}

/*--- loop() ---------------------------------------------------------------------------
  Main loop called by the Arduino framework
  --------------------------------------------------------------------------------------*/
void loop() {

}
