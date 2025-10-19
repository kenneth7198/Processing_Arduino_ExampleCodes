// 功能：讀取電容觸摸感測器狀態，並發布到 MQTT broker

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
const char* MQTT_HOST = "192.168.100.200";    // MQTT Broker IP 自己的電腦 IP, 根據自己組別配置
const int   MQTT_PORT = 1883;                 // MQTT Port
const char* DEVICE_ID = "esp32-touch-01";     // 設備 ID（每台設備要不同, 根據自己組別 01 02 03 04 ... 類推）

// 硬體設定
const int TOUCH_SENSOR_PIN = 12;              // 電容觸摸感測器接在 GPIO 12 (數位輸入，不受 WiFi 影響)

// 發送間隔
const unsigned long SEND_INTERVAL = 100;      // 每 0.1 秒檢查一次 (毫秒)

// MQTT 主題
const char* TOPIC_TOUCH = "sensor/touch";     // 發布觸摸感測器數據的主題
// ===============================================

WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastSendTime = 0;
int lastTouchState = HIGH;  // 記錄上次觸摸狀態

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32 電容觸摸感測器 MQTT 發送器 ===");
  
  // 設定 GPIO 12 為數位輸入
  pinMode(TOUCH_SENSOR_PIN, INPUT_PULLUP);
  
  Serial.print("觸摸感測器腳位: GPIO ");
  Serial.println(TOUCH_SENSOR_PIN);
  Serial.println();
  
  // 測試讀取
  int testValue = digitalRead(TOUCH_SENSOR_PIN);
  Serial.print("初始狀態: ");
  Serial.println(testValue ? "未觸摸 (HIGH)" : "觸摸中 (LOW)");
  Serial.println();

  // 連接 WiFi
  connectWiFi();
  
  // 設定 MQTT
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  
  // 連接 MQTT Broker
  connectMQTT();
  
  Serial.println("初始化完成，開始監測觸摸感測器...\n");
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
  
  // 每隔指定時間檢查觸摸感測器
  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    sendTouchData();
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
      Serial.println(TOPIC_TOUCH);
    } else {
      Serial.print("\nMQTT 連接失敗，錯誤代碼: ");
      Serial.println(mqtt.state());
      Serial.println("5 秒後重試...");
      delay(5000);
    }
  }
}

// 讀取並發送觸摸感測器數據
void sendTouchData() {
  // ✅ 使用 digitalRead 讀取 GPIO 12（數位輸入）
  int touchState = digitalRead(TOUCH_SENSOR_PIN);
  
  // 只在狀態改變時發送（減少不必要的訊息）
  if (touchState != lastTouchState) {
    lastTouchState = touchState;
    
    // 準備訊息
    String message;
    String status;
    
    if (touchState == LOW) {
      // 觸摸感測器被觸發（大多數觸摸模組輸出 LOW）
      message = "1";  // 觸摸
      status = "觸摸";
    } else {
      // 未觸摸
      message = "0";  // 未觸摸
      status = "釋放";
    }
    
    // 發布到 MQTT Broker
    bool success = mqtt.publish(TOPIC_TOUCH, message.c_str());
    
    // 在序列監控器顯示
    if (success) {
      Serial.print("發送成功 [");
      Serial.print(TOPIC_TOUCH);
      Serial.print("] 狀態: ");
      Serial.print(status);
      Serial.print(" (");
      Serial.print(message);
      Serial.println(")");
    } else {
      Serial.println("發送失敗");
    }
  }
}

