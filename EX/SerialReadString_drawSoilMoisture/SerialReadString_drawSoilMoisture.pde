import processing.serial.*;
Serial myPort;  
String val;      
int lf = 10;
float num;
void setup() 
{
  size(400, 200);
  printArray(Serial.list());
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
}

void draw()
{
  background(5);
  if ( myPort.available() > 0) {  
    val = myPort.readStringUntil(lf);
    if(val != null){
      num = float(val);
      if(num > 0){
      println(num);
       text("Data:"+val,50,100);
       //ellipse(200,100,150,map(num,0,1023,10,200));
      }
    }    
  }
  myPort.clear();
}  