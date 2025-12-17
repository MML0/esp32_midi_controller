#include <esp_now.h>
#include <WiFi.h>

// ======= Pin Definitions =======
const int pushPins[]   = {2, 4, 5, 12};
const int togPins[]    = {13, 14, 15, 16};
const int analogPins[] = {32, 33, 34, 35, 36, 39};

const int numPush   = sizeof(pushPins)   / sizeof(pushPins[0]);
const int numTog    = sizeof(togPins)    / sizeof(togPins[0]);
const int numAnalog = sizeof(analogPins) / sizeof(analogPins[0]);

// ======= ESP-NOW Receiver MAC ======= 8C:4F:00:36:05:20
uint8_t receiverAddress[] = {0xE8, 0x6B, 0xEA, 0xCF, 0xFE, 0xA4};

// ======= Toggle State =======
int togState[4]     = {0, 0, 0, 0};
int togPrevRead[4] = {1, 1, 1, 1};
unsigned long togLastTime[4] = {0};
const unsigned long debounceMs = 150;

// ======= Packet Buffer =======
char packet[200];

// ======= ESP-NOW Callback =======
void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Optional debug
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW OK" : "ESP-NOW FAIL");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  initPins();

  // WiFi & ESP-NOW
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true);
  }

  esp_now_register_send_cb(OnSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    while (true);
  }
}

// ================= LOOP =================
void loop() {
  updateToggles();
  buildPacket(packet, sizeof(packet));
  sendPacket(packet);
  delay(50);   // ~20 Hz (stable for ESP-NOW)
}

// ================= FUNCTIONS =================

// ---- Pin init ----
void initPins() {
  for (int i = 0; i < numPush; i++) pinMode(pushPins[i], INPUT_PULLUP);
  for (int i = 0; i < numTog;  i++) pinMode(togPins[i],  INPUT_PULLUP);
  for (int i = 0; i < numAnalog; i++) pinMode(analogPins[i], INPUT);
}

// ---- Toggle logic (debounced) ----
void updateToggles() {
  for (int i = 0; i < numTog; i++) {
    int raw = digitalRead(togPins[i]);

    if (raw == LOW && togPrevRead[i] == HIGH &&
        millis() - togLastTime[i] > debounceMs) {
      togState[i] = !togState[i];
      togLastTime[i] = millis();
    }

    togPrevRead[i] = raw;
  }
}

// ---- Packet builder (CSV) ----
void buildPacket(char* buf, size_t len) {
  int pos = 0;

  // Push buttons
  for (int i = 0; i < numPush; i++) {
    pos += snprintf(buf + pos, len - pos, "%d,", digitalRead(pushPins[i]));
  }

  // Toggles
  for (int i = 0; i < numTog; i++) {
    pos += snprintf(buf + pos, len - pos, "%d,", togState[i]);
  }

  // Analog
  for (int i = 0; i < numAnalog; i++) {
    pos += snprintf(buf + pos, len - pos,
                    (i < numAnalog - 1) ? "%d," : "%d",
                    analogRead(analogPins[i]));
  }
}

// ---- Output layer ----
void sendPacket(const char* buf) {
  Serial.println(buf);

  esp_err_t result = esp_now_send(
    receiverAddress,
    (uint8_t*)buf,
    strlen(buf) + 1   // IMPORTANT: include null terminator
  );

  // Optional debug
  // if (result != ESP_OK) Serial.println("Send error");
}
