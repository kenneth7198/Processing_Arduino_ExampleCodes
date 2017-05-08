#include <PreciseLM35.h>
const int pinLM35 = A0;
PreciseLM35 lm35(pinLM35, DEFAULT);
void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.println(String(lm35.readCelsius()));
  delay(2000);
}
