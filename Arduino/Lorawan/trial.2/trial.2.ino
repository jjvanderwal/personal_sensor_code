#include <LoRaAT.h>
LoRaAT mdot;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mdot.begin();
  mdot.join();
  lora.join();

}

void loop() {
  // put your main code here, to run repeatedly:

}
