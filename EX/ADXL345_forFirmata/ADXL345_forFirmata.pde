import processing.serial.*;
Serial myPort;
String val;
String[] adxl345;
int lf = 10;
String x = "0.1";
String y = "0.1";
String z = "1.0";

void setup() {
  size(500, 500, P3D);
  printArray(Serial.list());
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  background(128); 
  if(myPort.available() > 0){
   val = myPort.readStringUntil(lf);
   adxl345 = split(val, ',');
   if(adxl345 != null){
     x = adxl345[0];
     y = adxl345[1];
     z = adxl345[2];
   }
  }
  
  print("X=" + x);
  print(" Y=" + y);
  println(" Z=" + z);
  
  translate(250,250,0);
  rotateX(map(float(x),-1.0,1.0, 0.0, 1.0));
  rotateY(map(float(y),-1.0,1.0, 0.0, 1.0));
  rotateZ(map(float(z),0.9,1.5, 0.0, 1.0));
  fill(204,102,0);
  box(100);
}