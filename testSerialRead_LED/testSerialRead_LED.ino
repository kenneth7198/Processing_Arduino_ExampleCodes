char val;
int ledPin = 4; 

void setup() {
  pinMode(ledPin, OUTPUT); 
  Serial.begin(9600); 
}
 
void loop() {
    while (Serial.available()) { 
      val = Serial.read(); 
    }
if (val == 'H') { 
   digitalWrite(ledPin, HIGH); 
} else {
   digitalWrite(ledPin, LOW); 
}
   delay(100); 
}

