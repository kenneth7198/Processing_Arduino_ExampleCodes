import processing.serial.*;

Serial myPort;  
boolean val = false;

void setup() 
{
  size(200, 200);
  printArray(Serial.list());
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
 
}

void mouseClicked(){
  if(val==true){
   background(255,204,0);
   val = !val ;
   myPort.write(1);
  }else{
   background(51);
   val = !val;
   myPort.write(0); 
  }
}