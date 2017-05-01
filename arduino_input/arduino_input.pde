import processing.serial.*;
import cc.arduino.*;

Arduino arduino;
int ADC_pin = 0;

void setup() {
  size(1023, 1023);
  printArray(Serial.list());
  arduino = new Arduino(this, Arduino.list()[3], 57600);
}

void draw() {
  background(4,79,111);
  stroke(84,145,158);
  ellipse(512, 512, arduino.analogRead(ADC_pin) , arduino.analogRead(ADC_pin) );
}