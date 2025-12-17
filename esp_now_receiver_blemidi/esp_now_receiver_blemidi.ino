#include <esp_now.h>
#include <WiFi.h>

char packet[200];

// ================= ESP-NOW receive callback =================
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  // Print sender MAC
  // Serial.print("From MAC: ");
  // for (int i = 0; i < 6; i++) {
  //   Serial.printf("%02X", info->src_addr[i]);
  //   if (i < 5) Serial.print(":");
  // }
  // Serial.println();

  // Copy and print packet
  if (len >= sizeof(packet)) len = sizeof(packet) - 1;
  memcpy(packet, data, len);
  packet[len] = 0;  // null terminate
  // Serial.print("Packet: ");
  Serial.println(packet);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);          // STA mode for ESP-NOW
  delay(100);                   // short delay to ensure MAC is ready

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while(true);
  }

  // Register receive callback (new signature)
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP-NOW Receiver ready.");
}


// ================= LOOP =================
void loop() {
  // delay(10); // nothing else needed, callback handles packets
}
