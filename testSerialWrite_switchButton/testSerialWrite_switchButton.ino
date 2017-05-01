int switchPin = 5;                      

void setup() {
  pinMode(switchPin, INPUT);            
  Serial.begin(9600);                    
}

void loop() {
  if (digitalRead(switchPin) == HIGH) { 
    Serial.write(1);               
  } else {                        
    Serial.write(0);               
  }
  delay(100);                     
}

