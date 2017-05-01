import processing.serial.*;
import cc.arduino.*;

Arduino arduino;
int ADC_pin = 0;
int val = 0;

void setup() {
  size(500, 500);
  printArray(Serial.list());
  arduino = new Arduino(this, Arduino.list()[3], 57600);
}

void draw() {
  println(arduino.analogRead(ADC_pin));
  val = arduino.analogRead(ADC_pin);
  if (val >500){
   
  }
}