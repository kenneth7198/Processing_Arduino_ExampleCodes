import processing.serial.*;
Serial myPort;  
String val;      
int lf = 10;
void setup() {
  size(200, 200);
  printArray(Serial.list());
  String portName = Serial.list()[4];
  myPort = new Serial(this, portName, 9600);
}
void draw(){
  if ( myPort.available() > 0) {  
    val = myPort.readStringUntil(lf);
    if(val != null){
      print(val);
    }    
  }
  myPort.clear();
} 