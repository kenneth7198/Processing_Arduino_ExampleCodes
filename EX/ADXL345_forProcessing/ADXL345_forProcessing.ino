#include <Wire.h>
/*
   ADXL345 可以選擇使用 I2C 或 SPI協定來傳遞資料
   Arduino的類比腳使用 I2C 協定
   所以這裡我們會使用到的腳位是GND、VCC、CS、SDA和SCL
   VCC接CS =  I2C ; VCC接SDO = SPI
   使用兩顆10K歐姆電阻
   SDO、INT1、INT2不使用
*/

int I2C_Address = 0xA7 >> 1; //ADZL345的I2C位址
int X0, X1, Y0, Y1, Z0, Z1; //三軸加速度感測初始值變數
float X, Y, Z; //轉譯後顯示三軸加速度數值的變數

void setup() {
  Serial.begin(9600);
 // Serial.println("Start : \n -------------------");
  Wire.begin(); //初始化 I2C;
  setReg(0x2D, 0xA); //(打開電源，設定輸出資料速度為100Hz)
}

void loop() {
  // X Dimension
  X0 = getData(0x32); //取得 X軸低位元資料
  X1 = getData(0x33); //取得 X軸高位元資料
  X = ((X1 << 8) + X0) / 256.0;
  //Y  Dimension
  Y0 = getData(0x34); //取得 Y軸低位元資料
  Y1 = getData(0x35); //取得 Y軸高位元資料
  Y = ((Y1 << 8) + Y0) / 256.0;
  //Z  Dimension
  Z0 = getData(0x36); //取得 Z軸低位元資料
  Z1 = getData(0x37); //取得 Z軸高位元資料山
  Z = ((Z1 << 8) + Z0) / 256.0;
  //print
  Serial.print(X);
  Serial.print(",");
  Serial.print(Y);
  Serial.print(",");
  Serial.print(Z);
  Serial.println(",");
  delay(50);
}

/*
   setReg(reg,data):寫入佔存器
   參數:
   reg ⇨ 指定佔存器位置
   data ⇨ 要寫入的資料
*/

void setReg(int reg, int data) {
  Wire.beginTransmission(I2C_Address);
  Wire.write(reg); //指定佔存器
  Wire.write(data); //寫入資料
  Wire.endTransmission();
}
/*
   getData(reg): 取得佔存器中的資料
   參數:reg -- 佔存器位址
*/
int getData(int reg)
{ Wire.beginTransmission(I2C_Address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(I2C_Address, 1);
  if (Wire.available() <= 1)
  {
    return Wire.read();
  }
}

