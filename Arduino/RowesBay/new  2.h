#include <Arduino.h>
#include <String>
int numSensors = 10; //define the number of sensors


char validID[15][17] = {
	"2828F407030000D2",
	"2858200803000091",
	"287C2408030000E4",
	"28D2FD070300009C",
	"2832F80703000067",
	"28FA21080300006E",
	"2825BB26030000E1",
	"28DDA82603000000",
	"2863F50703000092",
	"28E71908030000C5",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000"
}; //identify valid sensor ids and ensure they are in order from tree top to bottom

double temperature[15] = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20}; //set the initial temperatures
int pinRed[15] = {54,55,56,57,58,59,60,61,62,63,64,65,66,67,68}; // these are the red pins
int pinBlue[15] = {44,45,46,3,4,5,6,7,8,9,10,11,12,13,69}; // these are the red pins

char inData[80]; //store the input data
byte index = 0;
int sIndex = 0;
/*
float tMin = 0.0;
float tMax = 9999.0;

// maximum PWM duty cycle, i. e. max LED brightness
const uint8_t maxBrightness = 100;

// set LED n off / on, color being controlled by colorIndex
void setLED(uint8_t n, bool on = false, uint8_t colorIndex = 0) {
	const uint8_t pinR = pins[2 * n];
	const uint8_t pinB = pins[2 * n + 1];
	analogWrite(pinRed, on ? rampR[colorIndex] : 0);
	analogWrite(pinBlue, on ? rampB[colorIndex] : 0);
}

uint8_t temp2index(const float& temp) {
	return (tMin == tMax) ? (numLeds / 2) : numLeds * (temp - tMin) / (tMax - tMin);
}
*/
void setup() {
	Serial.begin(57600);
	Serial1.begin(9600);
	Serial1.println("starting");
}  // end of setup

int count_commas(char s[]) {
	int count = 0;
	for (int i = 0; i < 80; i++) if (s[i] == ',') count++;
	return count;
}

int sensor_index(char s[]) {
	Serial1.print(s);
	int count = numSensors;
	for (int i = 0; i < numSensors; i++) {
		if (strcmp (s,validID[i]) == 0) {
			count=i;
		}
	}
	Serial1.print(" -- ");
	Serial1.print(count);
	return count;
}

void loop() {
	while(Serial.available() > 0) {
		char aChar = Serial.read();
		if(aChar == '\n') { //Parse here
			Serial1.println("parsing");

			if (count_commas(inData)==2) { //ensure there is only 2 commas time,id,temperature
				char *token = NULL;
				token = strtok(inData, ",");
				token = strtok(NULL, ","); // Use NULL to continue parsing the same string 
				if(token) sIndex = sensor_index(token); //get the sensor id index position
				if(sIndex<numSensors) {
					token = strtok(NULL, ","); // Use NULL to continue parsing the same string 
					if(token) {
						double temp = atof(token);
						temperature[sIndex] = temp;
						//tMin = (tMin < t) ? tMin : t;
						//tMax = (tMax > t) ? tMax : t;
						Serial1.print(" ... Temperature is ");
						Serial1.println(temp);
					}
				} else { 
					Serial1.println(" ... match failed "); 
				}
			} //End Parse
			for (int i = 0; i<80; i++) inData[index] = NULL;
			index = 0;
		} else {
			inData[index] = aChar;
			index++;
			inData[index] = '\0'; //Keep NULL Terminated
		}
	}
	

}