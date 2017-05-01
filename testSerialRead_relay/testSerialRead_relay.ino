int val;
int relayPin = 12;

void setup() {
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  while (Serial.available()) {
    val = Serial.read();
  }

  if (val == 1) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }

}
