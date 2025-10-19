#include <WiFi.h>

//////////////////// USER CONFIG ////////////////////
const char* WIFI_SSID     = "NetArt";
const char* WIFI_PASSWORD = "1qaz2wsx";

// Static IP Configuration for ESP32
const char* DEVICE_HOSTNAME = "ESP32-Group1";  // <--- 設備名稱，可在區網內識別，修改成自己組別
IPAddress local_IP(192, 168, 100, 201);      // <--- ESP32 的固定 IP, 請各組的自己修改自己的，不要跟人家衝突
IPAddress gateway(192, 168, 100, 1);         // <--- 路由器閘道 IP
IPAddress subnet(255, 255, 255, 0);          // <--- 子網路遮罩
IPAddress primaryDNS(8, 8, 8, 8);            
IPAddress secondaryDNS(8, 8, 4, 4);          

// Set this to the IP address of the computer running Processing
const char* SERVER_IP   = "192.168.100.200";   // <--- 改成你的電腦 IP, 請各組的自己修改自己的，不要跟人家衝突
const uint16_t SERVER_PORT = 10002;          // Processing 伺服器的埠
/////////////////////////////////////////////////////

WiFiClient client;

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL_MS = 2000;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);

  // Set device hostname
  WiFi.setHostname(DEVICE_HOSTNAME);
  Serial.print("Hostname set to: ");
  Serial.println(DEVICE_HOSTNAME);
  
  // Configure static IP before connecting
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured successfully");
  }
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    if (millis() - start > 20000) {  // 20s timeout then retry
      Serial.println("\nWi-Fi connect timeout, retrying...");
      WiFi.disconnect(true);
      delay(1000);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      start = millis();
    }
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.print("Subnet: ");
  Serial.println(subnet);
}

bool connectServer() {
  Serial.print("Connecting to server ");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  // Optional: set a short timeout for connect
  client.setTimeout(3000); // 3s

  if (client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("Server connected.");
    // 第一包先送一個識別訊息
    client.print("ESP32 connected: ");
    client.print(WiFi.localIP());
    client.print(" @ ");
    client.println(millis());
    return true;
  } else {
    Serial.println("Server connect failed.");
    return false;
  }
}

void ensureConnections() {
  // Ensure Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi dropped. Reconnecting...");
    connectWiFi();
  }
  // Ensure TCP
  if (!client.connected()) {
    Serial.println("TCP disconnected. Reconnecting...");
    client.stop();
    delay(500);
    connectServer();
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("\n=== ESP32 TCP Client for Processing ===");
  connectWiFi();
  connectServer();

  Serial.println("Type in Serial and press Enter to send to Processing server.");
}

void loop() {
  ensureConnections();

  // 1) Heartbeat every 2 seconds
  unsigned long now = millis();
  if (client.connected() && (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS)) {
    lastHeartbeat = now;
    // 每次送一行，以 '\n' 結尾：Processing 的 readString() 
    client.print("Heartbeat from ESP32 @ ms=");
    client.print(now);
    client.print(", RSSI=");
    client.print(WiFi.RSSI());
    client.print(", IP=");
    client.print(WiFi.localIP());
    client.print("\n");
  }

  // 2) Forward Serial input (line-based) to server
  //    在序列監控視窗選「Both NL & CR」或「Newline」
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // 確保換行結尾
    if (client.connected()) {
      client.print(line);
      client.print('\n');
    }
  }

  // 3) (Optional) 如果伺服器也回資料，可讀取並顯示
  if (client.connected() && client.available()) {
    String fromServer = client.readString(); // 讀取目前緩衝資料
    Serial.print("[From Server] ");
    Serial.print(fromServer);
  }

  delay(10); // 小緩衝
}

