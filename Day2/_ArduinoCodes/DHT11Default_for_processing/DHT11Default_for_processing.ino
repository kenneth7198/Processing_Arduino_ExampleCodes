#include <SimpleDHT.h>
int pinDHT11 = 2;
SimpleDHT11 dht11;
void setup() {
  Serial.begin(9600);
}
void loop() {
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.println("0.0");
    return;
  }
  Serial.println(temperature); 
  delay(1000);
}
