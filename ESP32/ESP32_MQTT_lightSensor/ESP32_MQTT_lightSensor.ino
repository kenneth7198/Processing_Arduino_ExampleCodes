// 功能：每隔 1 秒讀取光敏電阻數值，並發布到 MQTT broker

#include <WiFi.h>
#include <PubSubClient.h>

// ==================== 設定區 ====================
// WiFi 設定
const char* WIFI_SSID = "NetArt";
const char* WIFI_PASS = "1qaz2wsx";

// 固定 IP 設定（必須設定）
IPAddress local_IP(192, 168, 100, 201);       // ESP32 的固定 IP
IPAddress gateway(192, 168, 100, 1);          // 路由器閘道
IPAddress subnet(255, 255, 255, 0);           // 子網路遮罩
IPAddress primaryDNS(8, 8, 8, 8);             // DNS 伺服器

// MQTT Broker 設定
const char* MQTT_HOST = "192.168.100.200";      // MQTT Broker IP 自己的電腦 IP, 根據自己組別配置
const int   MQTT_PORT = 1883;                 // MQTT Port
const char* DEVICE_ID = "esp32-light-01";     // 設備 ID（每台設備要不同, 根據自己組別 01 02 03 04 ... 類推）

// 硬體設定
int LIGHT_SENSOR_PIN = 34;              // 光敏電阻重新接在 GPIO 34 (ADC1，因ADC2在WiFi啟動時會被關閉)

// 發送間隔
const unsigned long SEND_INTERVAL = 100;     // 每 0.1 秒發送一次 (毫秒)

// MQTT 主題
const char* TOPIC_LIGHT = "sensor/light";     // 發布光敏電阻數據的主題
// ===============================================

WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32 光敏電阻 MQTT 發送器 ===");
  
  // 設定 ADC 解析度和衰減
  analogReadResolution(12);  // 12-bit 解析度 (0-4095)
  analogSetAttenuation(ADC_11db);  // 設定衰減，讀取 0-3.3V 全範圍

  // 連接 WiFi
  connectWiFi();
  
  // 設定 MQTT
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  
  // 連接 MQTT Broker
  connectMQTT();
  
  Serial.println("初始化完成，開始發送光敏電阻數據...\n");
}

void loop() {
  // 確保 WiFi 和 MQTT 保持連線
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  
  if (!mqtt.connected()) {
    connectMQTT();
  }
  
  mqtt.loop();  // 處理 MQTT 訊息
  
  // 每隔指定時間發送光敏電阻數據
  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    sendLightData();
  }
}

// 連接 WiFi（使用固定 IP）
void connectWiFi() {
  Serial.print("正在連接 WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  
  // 設定固定 IP（必須在 WiFi.begin 之前）
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    Serial.println("固定 IP 設定失敗！");
  } else {
    Serial.println("固定 IP 設定成功");
  }
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi 連接成功！");
  Serial.print("IP 位址: ");
  Serial.println(WiFi.localIP());
  Serial.print("閘道: ");
  Serial.println(gateway);
  Serial.print("訊號強度 (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

// 連接 MQTT Broker
void connectMQTT() {
  Serial.print("正在連接 MQTT Broker...");
  
  while (!mqtt.connected()) {
    Serial.print(".");
    
    if (mqtt.connect(DEVICE_ID)) {
      Serial.println("\nMQTT 連接成功！");
      Serial.print("設備 ID: ");
      Serial.println(DEVICE_ID);
      Serial.print("發布主題: ");
      Serial.println(TOPIC_LIGHT);
    } else {
      Serial.print("\nMQTT 連接失敗，錯誤代碼: ");
      Serial.println(mqtt.state());
      Serial.println("5 秒後重試...");
      delay(5000);
    }
  }
}

// 讀取並發送光敏電阻數據
void sendLightData() {
  // 讀取光敏電阻 ADC 值 (0-4095)
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  // Serial.print(lightValue);
  // 簡單方法：直接發送數值（轉成字串）
  String message = String(lightValue);
  
  // 發布到 MQTT Broker
  bool success = mqtt.publish(TOPIC_LIGHT, message.c_str());
  
  // 在序列監控器顯示
  if (success) {
    Serial.print("發送成功 [");
    Serial.print(TOPIC_LIGHT);
    Serial.print("] 光敏值: ");
    Serial.println(lightValue);
  } else {
    Serial.println("發送失敗");
  }
}
