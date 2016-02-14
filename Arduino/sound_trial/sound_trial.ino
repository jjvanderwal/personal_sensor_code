//these define the pin connections
const int microphonePin= A0; //the microphone positive terminal will connect to analog pin A0 to be read
//const int ledPin=13; //the code will flash the LED connected to pin 13

int sample; //the variable that will hold the value read from the microphone each time

//const int threshold= 800;//the microphone threshold sound level at which the LED will turn on

void setup() {
//  pinMode (ledPin, OUTPUT);//sets digital pin 13 as output
  Serial.begin(115200); //sets the baud rate at 9600 so we can check the values the microphone is obtaining on the Serial Monitor
}

void loop(){
  sample= analogRead(microphonePin); //the arduino takes continuous readings from the microphone
  Serial.println(sample);
} 
