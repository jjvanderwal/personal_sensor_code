/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * initial work is with Wemos D1 mini and then will work with Lorawan 
  * here we want temperature and soil moisture reported.
  * GNU General Public License .. feel free to use / distribute ... no warranties
 */

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//define and initialize some of the pins/types 
#define ONE_WIRE_BUS D4 // Data wire is plugged into pin 2 on the Arduino
 
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
int deviceCount; //this is for a device count

 
//WiFi information
const char* ssid = "NETGEAR18";         //set the wifi id
const char* password = "FamilyJADE";    //define the wifi password

// ThingSpeak Settings
String APIKey = "R73HJN1DE9YPRD4V";             // enter your channel's Write API Key
const int updateThingSpeakInterval = 15 * 1000; // 50 second interval at which to update ThingSpeak

// function prototypes
void connectWiFi();

// Variable Setup
long lastConnectionTime = 0;        //define the last connection time so that we can count down until the next thingspeak upload

// setup the start
void setup() {
  Serial.begin(115200);
  sensors.begin();
 

}

//the looping script
void loop() {
  
  //confirm the wifi is still connected
  if (WiFi.status() != WL_CONNECTED) { connectWiFi(); }
  
  //run this if the time has hit the updateThingSpeakInterval
  if (millis() - lastConnectionTime > updateThingSpeakInterval) {

    String postData = (""); //define the initial post data
    
    deviceCount = sensors.getDeviceCount(); //get a count all all ds18b20s
    sensors.requestTemperatures(); // Send the command to get temperatures
    String Temp1 = String(sensors.getTempCByIndex(0));
    postData += ("field1=" + Temp1);

    String Soil1 = String(analogRead(A0));
    postData += ("&field2=" + Soil1);
        
    Serial.println(postData);    //print the post data to the serial port
    
    // Update ThingSpeak
    WiFiClient client; // use WiFiClient class to create TCP connections
    if (client.connect("api.thingspeak.com", 80)) {
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postData.length());
      client.print("\n\n");
      client.print(postData);
      String line = client.readStringUntil('\r');   // read all the lines of the reply from server and print them to Serial
      Serial.println(line);
    }
    client.stop();
    lastConnectionTime = millis(); // reset the last connection time
  }
  
  delay(5); // delay to let things reset
}

//section to start the WIFI
void connectWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}







