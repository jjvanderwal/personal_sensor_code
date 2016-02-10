#include <Arduino.h>
#include <String>

const int numSensors = 12; //define the number of sensors

//identify valid sensor ids and ensure they are in order from tree top to bottom



char validID[15][17] = {
	"2828F407030000D2",
	"2863F50703000092",
	"28D2FD070300009C",
	"2832F80703000067",
	"28E71908030000C5",
	"2825BB26030000E1",
	"28FA21080300006E",
	"2858200803000091",
	"28DDA82603000000",
	"287C2408030000E4",
	"289ECA26030000BE",
	"286F3C08030000A4",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000"
}; 

double temperature[15] = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20}; //set the initial temperatures
int pinRed[15] = {54,55,56,57,58,59,60,61,62,63,64,65,66,67,68}; // these are the red pins
int pinBlue[15] = {44,45,46,3,4,5,6,7,8,9,10,11,12,13,69}; // these are the red pins

char inData[80]; //store the input data
byte index = 0; byte sIndex = 0; // setup the index values
byte num_comma = 0; //count num comma
	
float tMin = 9999.0; //set the base tmin
float tMax = 0.0; //set the base tmax
const int maxBrightness = 175; // maximum PWM duty cycle, i. e. max LED brightness

uint8_t rampR[numSensors]; uint8_t rampB[numSensors]; //color ramps for blue and red

// set LED n off / on, color being controlled by colorIndex
void setLED(uint8_t n, bool on = false, uint8_t colorIndex = 0) {
	///*
	Serial1.print("n -- ");
	Serial1.print(n);
	Serial1.print(" ... color index -- ");
	Serial1.print(colorIndex);
	Serial1.print(" ... red -- ");
	Serial1.print(rampR[colorIndex]);
	Serial1.print(" ... blue -- ");
	Serial1.println(rampB[colorIndex]);
	//*/
	analogWrite(pinRed[n], on ? rampR[colorIndex] : 0);
	analogWrite(pinBlue[n], on ? rampB[colorIndex] : 0);
}

// index for temperatures
uint8_t temp2index(double temp1) {
	Serial1.print("temperature -- ");
	Serial1.print(temp1);
	Serial1.print(" .. tMin -- ");
	Serial1.print(tMin);
	Serial1.print(" .. tMax -- ");
	Serial1.print(tMax);
	Serial1.print(" .. index -- ");
	Serial1.println(numSensors * (temp1 - tMin) / (tMax - tMin));


	return (tMin == tMax) ? (numSensors / 2) : numSensors * (temp1 - tMin) / (tMax - tMin);
}

//function to identify the index position of the sensor
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

//check for end humidity line to dea with leds
const char check[16] = "  END HUMIDITY"; //this is hte check for char
int char_match(char s[]) {
	int count=0;
	for (int i = 3; i < 8; i++) if (s[i] == check[i]) count++;
	return(count);
}

//initial setup
void setup() {
	Serial.begin(57600);
	Serial1.begin(9600);
	Serial1.println("starting");
	// set LED pins as outputs:
	for (int i = 0; i < numSensors; i++) {
		pinMode(pinRed[i], OUTPUT);
		pinMode(pinBlue[i], OUTPUT);
	}
	// set up color ramps
	float f = 1.0 * (255) / (numSensors-1);
	for (int i = 0; i < numSensors; i++) {
		rampB[i] = (uint8_t)((numSensors - i - 1) * f); // blue: maxBrightness down to 0, numSensors steps
	}
	f = 1.0 * (175 - 124) / (numSensors-1);
	for (int i = 0; i < numSensors; i++) {
		rampR[i] = (uint8_t)(i * f + 124); // red: 127 to maxBrightness, numSensors steps
	}

	// Blink each LED red and blue in turn
	for (uint8_t i = 0; i < numSensors; i++) {
		setLED(i, true, numSensors);
		delay(100);
		setLED(i, true, 0);
		delay(100);
		setLED(i, false);
	}
	delay(500);

	// Show full gradient, increasing temp with LED no
	for (uint8_t i = 0; i < numSensors; i++) {
			setLED(i, true, i);
	}
	delay(2000);
	
	
}  // end of setup


void loop() {
	while(Serial.available() > 0) {
		char aChar = Serial.read();
		if (aChar==',') num_comma++; //track number of commas
		if(aChar == '\n') { //Parse here
			Serial1.print("parsing -- ");
			//Serial1.print(count_commas(inData));
			//Serial1.print(" -- ");
			//Serial1.println(inData);
			if (char_match(inData)==5) { //if we have end humidity... recolor the lights
				Serial1.print("working on leds -- ");
				tMin = 9999.0; //set the base tmin
				tMax = 0.0; //set the base tmax
				for (uint8_t i = 0; i < numSensors; i++) { //reset tmin and tmax
					if (tMin > temperature[i]) tMin = temperature[i];
					if (tMax < temperature[i]) tMax = temperature[i];
				}		
				for (uint8_t i = 0; i < numSensors; i++) {
					/*
					Serial1.print("temperature -- ");
					Serial1.print(temperature[i]);
					Serial1.print(" .. index -- ");
					Serial1.println(temp2index(temperature[i]));
					*/
					setLED(i, i < numSensors, temp2index(temperature[i]));
				}
			}
			if (num_comma==2) { //ensure there is only 2 commas time,id,temperature
			//if (count_commas(inData)==2) { //ensure there is only 2 commas time,id,temperature
				char *token = NULL;
				token = strtok(inData, ",");
				token = strtok(NULL, ","); // Use NULL to continue parsing the same string 
				if(token) sIndex = sensor_index(token); //get the sensor id index position
				if(sIndex<numSensors) {
					token = strtok(NULL, ","); // Use NULL to continue parsing the same string 
					if(token) {
						double temp = atof(token);
						if (sIndex>9) temp = temp+0.63;
						temperature[sIndex] = temp;
						Serial1.print(" ... Temperature is ");
						Serial1.println(temp);
					}
				} else { 
					Serial1.println(" ... match failed "); 
				}
			} //End Parse
			for (int i = 0; i<80; i++) {
				inData[i] = '\0';
			}
			index = 0;
			num_comma=0; //count num comma
		} else {
			inData[index] = aChar;
			index++;
			inData[index] = '\0'; //Keep NULL Terminated
		}
	}
	

}