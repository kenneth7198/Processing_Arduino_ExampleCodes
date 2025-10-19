/*
 * ESP32 MQTT LED Control (Subscriber)
 * 
 * 功能說明：
 * - ESP32 作為 MQTT 訂閱者 (Subscriber)
 * - 接收 JSON 格式的控制指令
 * - 根據 JSON 內容控制 LED 燈開關
 * 
 * JSON 格式範例：
 * {"GPIO": "on"}  // 開啟 LED
 * {"GPIO": "off"}  // 關閉 LED
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // 需要安裝 ArduinoJson 函式庫

// ========== WiFi 設定 ==========
const char* WIFI_SSID = "NetArt";
const char* WIFI_PASSWORD = "1qaz2wsx";

// ========== 靜態 IP 設定 ==========
IPAddress local_IP(192, 168, 100, 201);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

// ========== MQTT 設定 ==========
const char* MQTT_BROKER = "192.168.100.200";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32_LED_Control";
const char* TOPIC_LED_CONTROL = "sensor/LED";      // 訂閱主題：接收控制指令用 on 或 off
const char* TOPIC_LED_STATUS = "status/led";       // 發布主題：回報LED狀態 GPIO23 的現況 on 或 off, 以及pin腳, 時間戳記

// ========== 硬體腳位設定 ==========
const int LED_PIN = 23;     //紅色LED
const int BUTTON_PIN = 22;  // 手動操作回應用, 可手動按下後發送MQTT訊息到 broker

// ========== 全域變數 ==========
WiFiClient espClient;
PubSubClient mqtt(espClient);
int currentLedState = LOW;
int lastButtonState = HIGH;

// ========== 函式宣告 ==========
void setupWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void controlLED(int state);
void publishLEDStatus();

void setup() {
  // 初始化序列埠
  Serial.begin(115200);
  Serial.println("\n=== ESP32 MQTT LED Control ===");
  
  // 設定 GPIO
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);
  
  // 連接 WiFi
  setupWiFi();
  
  // 設定 MQTT
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  
  Serial.println("初始化完成！");
  Serial.println("等待 MQTT 控制指令...");
  Serial.printf("訂閱主題: %s\n", TOPIC_LED_CONTROL);
}

void loop() {
  // 確保 MQTT 連線
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();
  
  // ESP32上面有連接的按鈕控制（用於測試）
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);  // 去彈跳
    currentLedState = !currentLedState;
    controlLED(currentLedState);
    publishLEDStatus();
    Serial.println("本地按鈕觸發");
  }
  lastButtonState = buttonState;
  
  delay(10);
}

// ========== WiFi 連線函式 ==========
void setupWiFi() {
  Serial.print("連接到 WiFi: ");
  Serial.println(WIFI_SSID);
  
  // 設定靜態 IP
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("靜態 IP 設定失敗");
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi 連線成功！");
    Serial.print("IP 位址: ");
    Serial.println(WiFi.localIP());
    Serial.print("訊號強度: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nWiFi 連線失敗！");
  }
}

// ========== MQTT 重新連線函式 ==========
void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("連接到 MQTT Broker...");
    
    if (mqtt.connect(MQTT_CLIENT_ID)) {
      Serial.println("成功！");
      
      // 訂閱控制主題
      mqtt.subscribe(TOPIC_LED_CONTROL);
      Serial.printf("已訂閱主題: %s\n", TOPIC_LED_CONTROL);
      
      // 發送上線訊息
      publishLEDStatus();
      
    } else {
      Serial.print("失敗，錯誤碼 = ");
      Serial.print(mqtt.state());
      Serial.println(" 5秒後重試...");
      delay(5000);
    }
  }
}

// ========== MQTT 訊息接收回調函式 ==========
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n--- 收到 MQTT 訊息 ---");
  Serial.printf("主題: %s\n", topic);
  
  // 將 payload 轉換為字串
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.printf("內容: %s\n", message.c_str());
  
  // 解析 JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("JSON 解析失敗: ");
    Serial.println(error.c_str());
    return;
  }
  
  // 檢查是否包含 "GPIO23" 欄位
  if (doc.containsKey("GPIO23")) {
    String ledCommand = doc["GPIO23"];
    Serial.printf("GPIO23 控制指令: %s\n", ledCommand.c_str());
    
    // 判斷 "on" 或 "off"
    int ledValue = 0;
    if (ledCommand == "on" || ledCommand == "ON") {
      ledValue = 1;
    } else if (ledCommand == "off" || ledCommand == "OFF") {
      ledValue = 0;
    } else {
      Serial.printf("未知的指令: %s (請使用 'on' 或 'off')\n", ledCommand.c_str());
      return;
    }
    
    // 控制 LED
    controlLED(ledValue);
    
    // 回報狀態
    publishLEDStatus();
  } else {
    Serial.println("JSON 中沒有 'GPIO23' 欄位");
  }
  
  Serial.println("----------------------\n");
}

// ========== LED 控制函式 ==========
void controlLED(int state) {
  currentLedState = state;
  digitalWrite(LED_PIN, state ? HIGH : LOW);
  
  Serial.printf("LED 狀態: %s\n", state ? "ON" : "OFF");
}

// ========== 發布 LED 狀態函式 ==========
void publishLEDStatus() {
  StaticJsonDocument<200> doc;
  doc["GPIO23"] = currentLedState ? "on" : "off";
  doc["pin"] = LED_PIN;
  doc["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  mqtt.publish(TOPIC_LED_STATUS, jsonString.c_str());
  Serial.printf("已發布狀態: %s\n", jsonString.c_str());
}
