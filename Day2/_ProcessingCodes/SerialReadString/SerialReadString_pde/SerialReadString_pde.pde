import processing.serial.*;
Serial myPort;  
String val;      
int lf = 10;
float num;
void setup() {
  size(200, 200);
  printArray(Serial.list());
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
}
void draw(){
  if ( myPort.available() > 0) {  
    val = myPort.readStringUntil(lf);
    if(val != null){
      num = float(val);
      println("Temp:" + num);
    }    
  }
  myPort.clear();
} 