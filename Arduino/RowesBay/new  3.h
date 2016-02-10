// Do not remove the include below
#include "RGB_LED_Gradient.h"

#include <OneWire.h>
#include <DallasTemperature.h>

/*
  This sketch reads the temperatures off DS18B20 OneWire sensors,
  outputs these values via the serial port, and visualizes them
  via a set of R(G)B-LEDs using a relative red-blue gradient.

  The circuit:
  * Alternating red and blue LEDs attached from pins 44, 45, and 2 through 13 to ground (i. e. odd = red).
  * OneWire bus attached to pin 53.

  Adapted from a public domain example by Tom Igoe in 2009.

 */

const uint8_t oneWirePin = 53;

const uint8_t sensorAdresses[] = {
		0x28, 0x57, 0x62, 0x82, 0x04, 0x00, 0x00, 0x67,
		0x28, 0x61, 0x03, 0x82, 0x04, 0x00, 0x00, 0xE4,
		0x28, 0x52, 0xF6, 0x81, 0x04, 0x00, 0x00, 0xAC,
		0x28, 0x16, 0x94, 0x82, 0x04, 0x00, 0x00, 0x1F,
		0x28, 0x9D, 0x2F, 0x82, 0x04, 0x00, 0x00, 0x32,
		0x28, 0x53, 0x48, 0x82, 0x04, 0x00, 0x00, 0xFE,
		0x28, 0xF5, 0x9E, 0x82, 0x04, 0x00, 0x00, 0xD2
};

OneWire oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

int numSensors = 0;

const uint8_t pins[] = { 44, 45, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const uint8_t numPins = sizeof(pins) / sizeof(pins[0]);
const uint8_t numLeds = numPins / 2;

uint8_t rampR[numLeds];
uint8_t rampB[numLeds];

float tMin = 0.0;
float tMax = 0.0;

// maximum PWM duty cycle, i. e. max LED brightness
const uint8_t maxBrightness = 16;

// set LED n off / on, color being controlled by colorIndex
void setLED(uint8_t n, bool on = false, uint8_t colorIndex = 0) {
	const uint8_t pinR = pins[2 * n];
	const uint8_t pinB = pins[2 * n + 1];
	analogWrite(pinR, on ? rampR[colorIndex] : 0);
	analogWrite(pinB, on ? rampB[colorIndex] : 0);
}

uint8_t temp2index(const float& temp) {
	return (tMin == tMax) ? (numLeds / 2) : numLeds * (temp - tMin) / (tMax - tMin);
}

void setup() {
	Serial.begin(115200);
	Serial.println("#TCC Data Logger Boot");

	// Start up the 1wire Dallas library
  sensors.begin();

  // Find 1wire devices on the 1wire bus
  numSensors = sensors.getDeviceCount();

  Serial.print("#Sensors: ");
  Serial.println(numSensors);

  // set LED pins as outputs:
  for (uint8_t i = 0; i < numPins; ++i) {
    pinMode(pins[i], OUTPUT);
  }

  // set up color ramps
  float f = 1.0 * maxBrightness / numLeds;
	for (uint8_t i = 0; i < numLeds; ++i) {
		// red: 0 to maxBrightness, numLeds steps
		rampR[i] = (uint8_t)(i * f);
		// blue: maxBrightness down to 0, numLeds steps
		rampB[i] = (uint8_t)((numLeds - i - 1) * f);
	}

	// Show LED test patterns upon startup

  // Blink each LED red and blue in turn
	for (uint8_t i = 0; i < numLeds; i++) {
		setLED(i, true, numLeds);
		delay(100);
		setLED(i, true, 0);
		delay(100);
		setLED(i, false);
	}
	delay(500);

	// Set all LEDs to red (highest temperature)
	for (uint8_t i = 0; i < numLeds; i++) {
			setLED(i, true, numLeds);
	}
	delay(2000);

	// Show full gradient, increasing temp with LED no
	for (uint8_t i = 0; i < numLeds; i++) {
			setLED(i, true, i);
	}
	delay(2000);

	// Show full gradient, decreasing temp with LED no
	for (uint8_t i = 0; i < numLeds; i++) {
			setLED(i, true, numLeds - i);
	}
	delay(2000);
}

void loop() {
	static float temps[numLeds];

	sensors.begin();
	numSensors = min(sensors.getDeviceCount(), numLeds);

	sensors.requestTemperatures();
	for (int i = 0 ; i < numSensors ; i++ ) {
		const uint8_t* thisSensor = sensorAdresses + (i << 3);
		float t = temps[i] = sensors.getTempC(thisSensor);
		if (i == 0) {
			tMin = tMax = t;
		}
		tMin = (tMin < t) ? tMin : t;
		tMax = (tMax > t) ? tMax : t;
		Serial.print("Temp #");
		Serial.print(i);
		Serial.print(" / ");
		log_address(thisSensor);
		Serial.print(": ");
		Serial.println(t);
	}
	for (uint8_t i = 0; i < numLeds; i++) {
			setLED(i, i < numSensors, temp2index(temps[i]));
	}

	Serial.println();
//	delay(500);
}
