import processing.serial.*;
import cc.arduino.*;
Arduino arduino;
int x=0;
int y=0;
int z=0;

void setup() {
  size(500, 500, P3D);
  printArray(Arduino.list());
  arduino = new Arduino(this, Arduino.list()[3], 57600);
}

void draw() {
  background(128); 
  print("X=" + arduino.analogRead(0));
  print(" Y=" + arduino.analogRead(1));
  println(" Z=" + arduino.analogRead(2));
  x = arduino.analogRead(0);
  y = arduino.analogRead(1);
  z = arduino.analogRead(2);
  
  translate(250,250,0);
  rotateX(map(x,260,430, 0, 1));
  rotateY(map(y,260,430, 0, 1));
  rotateZ(map(z,390,450, 0, 1));
  fill(204,102,0);
  box(100);
}