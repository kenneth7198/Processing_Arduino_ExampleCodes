/*土壤濕度感測器 Soil Moisture Sensor */
// 設定以analog A0 腳位讀取濕度變化數值
int moistureSensor = 0;
int ledPin_1 = 8;
int ledPin_2 = 9;
int ledPin_3 = 10;
int ledPin_4 = 11;
int myTone = 12;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin_1, OUTPUT);
  pinMode(ledPin_2, OUTPUT);
  pinMode(ledPin_3, OUTPUT);
  pinMode(ledPin_4, OUTPUT);
  pinMode(myTone, OUTPUT);
}

void loop() {
  // 讀取濕度感測器數值
  int sensorValue = analogRead(moistureSensor);
  Serial.println(sensorValue);
  //依照水量分四個等級
  //濕度最低第一級亮一顆LED，如此類推
  //濕度最高時同時打開蜂鳴器＋繼電器
  delay(50);
  if (sensorValue <= 400) {
    digitalWrite(ledPin_1, HIGH);
    digitalWrite(ledPin_2, HIGH);
    digitalWrite(ledPin_3, HIGH);
    digitalWrite(ledPin_4, HIGH);
    tone(myTone, 440, 220);
  }

  if (sensorValue > 400 && sensorValue <= 500 ) {
    digitalWrite(ledPin_1, HIGH);
    digitalWrite(ledPin_2, HIGH);
    digitalWrite(ledPin_3, HIGH);
    digitalWrite(ledPin_4, LOW);
  }

  if (sensorValue > 500 && sensorValue <= 600 ) {
    digitalWrite(ledPin_1, HIGH);
    digitalWrite(ledPin_2, HIGH);
    digitalWrite(ledPin_3, LOW);
    digitalWrite(ledPin_4, LOW);
  }

  if (sensorValue > 600 && sensorValue <=1020) {
    digitalWrite(ledPin_1, HIGH);
    digitalWrite(ledPin_2, LOW);
    digitalWrite(ledPin_3, LOW);
    digitalWrite(ledPin_4, LOW);
  }

  else if (sensorValue >1020 ){ 
    digitalWrite(ledPin_1, LOW);
  }


}
