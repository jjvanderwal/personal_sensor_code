/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * GNU General Public License .. feel free to use / distribute ... no warranties
 */

#include <ESP8266WiFi.h>
#include "DHT.h"
#include <SparkFunTSL2561.h>
#include <Wire.h>

//define and initialize some of the pins/types 
#define DHTPIN_0 D3     // what pin we're connected to
#define DHTTYPE_0 DHT22   // DHT 22  (AM2302)
DHT dht_0(DHTPIN_0, DHTTYPE_0);
const int microphonePin = A0; //the microphone envelope terminal will connect to analog pin A0 to be read
SFE_TSL2561 light; // Create an SFE_TSL2561 object, here called "light":
 
//WiFi information
const char* ssid = "NETGEAR18";
const char* password = "FamilyJADE";

// ThingSpeak Settings
String APIKey = "H3OJHBYHU101EFYU";             // enter your channel's Write API Key
const int updateThingSpeakInterval = 50 * 1000; // 20 second interval at which to update ThingSpeak

// function prototypes
void connectWiFi();

// Variable Setup
long lastConnectionTime = 0;
int sound_max = 0; // variable to hold max sound value for each thingspeak iteration
long sound_count = 0; // variable to hold number of sound samples for each thingspeak iteration
long sound_sum = 0; // variable to hold sum of the sound value for each thingspeak iteration
int sound_sample; //the variable that will hold the value read from the microphone each time
boolean light_gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int light_ms;  // Integration ("shutter") time in milliseconds

// setup the start
void setup() {
  Serial.begin(115200);
  delay(10);
  
  //start the DHT22
  dht_0.begin(); 
  
  //start the light sensor
  light.begin(); 
  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)
  light_gain = 0;
  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration
  unsigned char time = 2;
  // setTiming() will set the third parameter (ms) to the requested integration time in ms (this will be useful later):
  light.setTiming(light_gain,time,light_ms);
  // To start taking measurements, power up the sensor:
  light.setPowerUp();
  
}

//the looping script
void loop() {
  //confirm the wifi is still connected
  if (WiFi.status() != WL_CONNECTED) { 
    delay(10);
    connectWiFi(); 
  } 
  
  //track sound information
  sound_sample= analogRead(microphonePin); //the arduino takes continuous readings from the microphone
  if (sound_max < sound_sample) {sound_max = sound_sample;} //keep the maximum value
  sound_sum += sound_sample; //sum up the sound values
  sound_count += 1;
    
  //run this if the time has hit the updateThingSpeakInterval
  if (millis() - lastConnectionTime > updateThingSpeakInterval) {

    String postData = (""); //define the initial post data
    
    //get the data from the DHT22
    float h = dht_0.readHumidity();
    float t = dht_0.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    } else {
      String S1 = String(h);
      String S2 = String(t);
      postData += ("field1=" + S1 + "&field2=" + S2);
    }
    
    //work with the sound data
    Serial.println(sound_count);
    String S3 = String(round(sound_sum / sound_count));
    String S4 = String(sound_max);
    if (postData == "") {
      postData += ("field3=" + S3 + "&field4=" + S4);
    } else {
      postData += ("&field3=" + S3 + "&field4=" + S4);
    }
    sound_sum = 0;
    sound_max = 0;
    sound_count = 0;
    
    //work with the light data
    unsigned int data0, data1;
    if (light.getData(data0,data1)) { // getData() returned true, communication was successful
      double lux;    // Resulting lux value
      boolean good;  // True if neither sensor is saturated
      good = light.getLux(light_gain,light_ms,data0,data1,lux); // Perform lux calculation:
      if (good) {
        String S5 = String(lux);
        if (postData == "") {
          postData += ("field5=" + S5);
        } else {
          postData += ("&field5=" + S5);
        }      
      } 
    }    
    
    Serial.println(postData);
    
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







