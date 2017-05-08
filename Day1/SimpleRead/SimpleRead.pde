import processing.serial.*;

Serial myPort;  
int val;      
void setup() 
{
  size(200, 200);
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
}
void draw()
{
  if ( myPort.available() > 0) {  
    val = myPort.read();         
  }
  background(255);             
  if (val == 0) {              
    fill(0);                   
  } 
  else {                       
    fill(204);                 
  }
  rect(50, 50, 100, 100);
}