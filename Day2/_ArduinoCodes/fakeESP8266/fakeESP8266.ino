void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("I'm a fake ESP-12F!");
  delay(2000);
}
