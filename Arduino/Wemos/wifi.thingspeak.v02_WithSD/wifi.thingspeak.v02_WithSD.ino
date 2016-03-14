/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * GNU General Public License .. feel free to use / distribute ... no warranties
 */

#include "ESP8266WiFi.h"
#include "DHT.h"
#include "SparkFunTSL2561.h"
#include "SFE_BMP180.h"
#include "Wire.h"

//define and initialize some of the pins/types 
#define DHTPIN_0 D3               // what pin we're connected to
#define DHTTYPE_0 DHT22           // DHT 22  (AM2302)
DHT dht_0(DHTPIN_0, DHTTYPE_0);   //define the DHT sensor as the DHT22
const int microphonePin = A0;     //the microphone envelope terminal will connect to analog pin A0 to be read
SFE_TSL2561 light;                // Create an SFE_TSL2561 object, here called "light":
SFE_BMP180 pressure;              //create an SFE_BMP180 object, here called "pressure"
#define ALTITUDE 20.0             // Altitude of Townsville in meters
const int chipSelect = D8;        // Set chipSelect
String dataString ="";            //Make string to push into CSV file
File sensorData;

 
//WiFi information
const char* ssid = "SSID HERE";         //set the wifi id
const char* password = "PASSWORD";    //define the wifi password

// ThingSpeak Settings
String APIKey = "API HERE";             // enter your channel's Write API Key
const int updateThingSpeakInterval = 30 * 1000; // 50 second interval at which to update ThingSpeak

// function prototypes
void connectWiFi();
void saveData();

// Variable Setup
long lastConnectionTime = 0;        //define the last connection time so that we can count down until the next thingspeak upload
int sound_max = 0;                  // variable to hold max sound value for each thingspeak iteration
long sound_count = 0;               // variable to hold number of sound samples for each thingspeak iteration
long sound_sum = 0;                 // variable to hold sum of the sound value for each thingspeak iteration
int sound_sample;                   //the variable that will hold the value read from the microphone each time
int DHT_temp;                    //ariable to hold temp
int DHT_hum;                      //Varaible for humidity
int Baro_read;                    //Variable for Baro
int light_read;                   //Variable for light
boolean light_gain;                 // Gain setting, 0 = X1, 1 = X16;
unsigned int light_ms;              // Integration ("shutter") time in milliseconds

// setup the start
void setup() {
  Serial.begin(115200);
  delay(10);


  //check if SD is connected properly
  if (!SD.begin(chipSelect)) {
    Serial.print("card read failed, please check and reset board");
    return;
  }
  pinMode(chipSelect, OUTPUT);

  //Start DHT
  
  //start the DHT22
  dht_0.begin(); 
  
  //start the light sensor
  light.begin(); 
  light_gain = 0;                               // If gain = false (0), device is set to low gain (1X)
                                                // If gain = high (1), device is set to high gain (16X)
  unsigned char time = 2;                       // If time = 0, integration will be 13.7ms
                                                // If time = 1, integration will be 101ms
                                                // If time = 2, integration will be 402ms
                                                // If time = 3, use manual start / stop to perform your own integration
  light.setTiming(light_gain,time,light_ms);    // setTiming() will set the third parameter (ms)
                                                // to the requested integration time in ms (this will be useful later):
  light.setPowerUp();                           // To start taking measurements, power up the sensor:
  
  //start the pressure sensor
  pressure.begin();
  dataString = "Time since boot(seconds),Temperatur(Degrees Celsius),Humidity(%),Barometer(HectoPascles),Light(Lux),Sound";
  saveData();
}

//the looping script
void loop() {
  
  //confirm the wifi is still connected
  if (WiFi.status() != WL_CONNECTED) { connectWiFi(); }
  
  //track sound information
  sound_sample= analogRead(microphonePin);                  //the arduino takes continuous readings from the microphone
  if (sound_max < sound_sample) {sound_max = sound_sample;} //keep the maximum value
  sound_sum += sound_sample;                                //sum up the sound values
  sound_count += 1;                                         //keep track of the number of samples
    
  //run this if the time has hit the updateThingSpeakInterval
  if (millis() - lastConnectionTime > updateThingSpeakInterval) {

    String postData = (""); //define the initial post data
    
    //get the data from the DHT22
    float h = dht_0.readHumidity();
    DHT_hum = dht_0.readHumidity();
    float t = dht_0.readTemperature();
    DHT_temp = dht_0.readTemperature();
    
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
    if (postData != "") { postData += ("&"); }
    postData += ("field3=" + S3 + "&field4=" + S4);
    sound_sum = 0; sound_max = 0; sound_count = 0;      //reset values to 0
    
    //work with the light data
    unsigned int data0, data1;
    if (light.getData(data0,data1)) {                           // getData() returned true, communication was successful
      double lux;                                               // Resulting lux value
      boolean good;                                             // True if neither sensor is saturated
      good = light.getLux(light_gain,light_ms,data0,data1,lux); // Perform lux calculation:
      if (good) {
        String S5 = String(lux);
        if (postData != "") { postData += ("&"); }
        postData += ("field5=" + S5);
        light_read = lux;
      } 
    }    
    
    //work with the pressure data
    char status;
    double T,P,p0;
    status = pressure.startTemperature();
    if (status != 0) {
      delay(status);                            // Wait for the measurement to complete:
      status = pressure.getTemperature(T);      // Retrieve the completed temperature measurement stored in the variable T.
      if (status != 0) {                        // Function returns 1 if successful, 0 if failure.
        status = pressure.startPressure(3);     // Start a pressure measurement:
                                                // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
                                                // If request is successful, the number of ms to wait is returned.
                                                // If request is unsuccessful, 0 is returned.
        if (status != 0) {
          delay(status);                        // Wait for the measurement to complete:
          status = pressure.getPressure(P,T);   // Retrieve the completed pressure measurement: the measurement is stored in the variable P.
          if (status != 0) {                    // Function returns 1 if successful, 0 if failure.
            p0 = pressure.sealevel(P,ALTITUDE); // p0 = sea-level compensated pressure in mb
            String S6 = String(p0);
            String S7 = String(T);
            if (postData != "") { postData += ("&"); }
            postData += ("field6=" + S6 + "&field7=" + S7);
            Baro_read = p0;
          }
        }
      }
    }
    CurTime = long(millis()/1000);
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
  //set data to save
  dataString = String(String(CurTime) + "," + String(DHT_temp) + "," + String(DHT_hum) + "," + String(Baro_read) + "," + String(light_read) + "," + String(sound_sample));
  saveData(); 
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

void saveData(){
if(SD.exists("data.csv")){ // check the card is still there
// now append new data file
sensorData = SD.open("data.csv", FILE_WRITE);
if (sensorData){
  sensorData.println(dataString);
  sensorData.close(); // close the file
  Serial.println(dataString);
  }
}
else{
  Serial.print("Error writing to file !");
  }
}





