import processing.serial.*;
import cc.arduino.*;
Arduino arduino;
void setup() {
  size(200, 200);
  printArray(Arduino.list());
  arduino = new Arduino(this, Arduino.list()[1], 57600);
}

void draw() {}
void keyPressed(){
 if(key == CODED){
  if(keyCode == UP){
     arduino.analogWrite(9, 255 );
     background(31,45,67);
  }else if(keyCode == DOWN){
     arduino.analogWrite(11, 255);
     background(99,100,0);
  }
 }
}