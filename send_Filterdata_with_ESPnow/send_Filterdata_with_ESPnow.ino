#include <esp_now.h>
#include <WiFi.h>

// ======= Pin Definitions =======
const int pushPins[]   = {4, 16, 17, 5};          
const int togPins[]    = {18, 19, 15, 21};       
const int analogPins[] = {32, 33, 34, 35, 36, 39};

const int numPush   = sizeof(pushPins) / sizeof(pushPins[0]);
const int numTog    = sizeof(togPins)  / sizeof(togPins[0]);
const int numAnalog = sizeof(analogPins) / sizeof(analogPins[0]);

int togState[4] = {0,0,0,0};
int togPrevRead[4] = {1,1,1,1};
int prevAnalog[6] = {0,0,0,0,0,0};   // مقادیر قبلی آنالوگ برای فیلتر

// ======= MAC گیرنده ======= D4:E9:F4:C2:F5:3C
// uint8_t receiverAddress[] = {0xE8, 0x6B, 0xEA, 0xCF, 0xFE, 0xA4};  // جای MAC گیرنده خودت
uint8_t receiverAddress[] = {0xD4, 0xE9, 0xF4, 0xC2, 0xF5, 0x3C};  // جای MAC گیرنده خودت

// ======= ارسال Callback =======
void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("Delivery Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// ======= خواندن آنالوگ با فیلتر و مقیاس =======
int readAnalogSmooth(int pin, int index, int threshold = 2) {
  int raw = analogRead(pin);
  int scaled = map(raw, 0, 4095, 0, 128);  // تبدیل به 0–100

  // --- استثناء: مقادیر خیلی پایین یا خیلی بالا همیشه ثبت شوند ---
  if (scaled <= 1 || scaled >= 128) {
    prevAnalog[index] = scaled;
    return scaled;
  }

  // --- فیلتر معمولی Threshold ---
  if (abs(scaled - prevAnalog[index]) >= threshold) {
    prevAnalog[index] = scaled;
  }

  return prevAnalog[index];
}


void setup() {
  Serial.begin(115200);

  for(int i=0; i<numPush; i++) pinMode(pushPins[i], INPUT_PULLUP);
  for(int i=0; i<numTog;  i++) pinMode(togPins[i],  INPUT_PULLUP);

  // WiFi & ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // ====== SOFTWARE TOGGLE LOGIC ======
  for(int i = 0; i < numTog; i++){
    int raw = digitalRead(togPins[i]);
    if(raw == LOW && togPrevRead[i] == HIGH){
      togState[i] = !togState[i];
    }
    togPrevRead[i] = raw;
  }

  // ====== BUILD PACKET ======
  String packet = "";

  // Push buttons
  for(int i=0; i<numPush; i++){
    packet += digitalRead(pushPins[i]);
    packet += ",";
  }

  // Software toggles
  for(int i=0; i<numTog; i++){
    packet += togState[i];
    packet += ",";
  }

  // Analog reads با فیلتر و مقیاس
  for(int i=0; i<numAnalog; i++){
    int val = readAnalogSmooth(analogPins[i], i);
    packet += val;
    if(i < numAnalog-1) packet += ",";
  }

  // ارسال با ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)packet.c_str(), packet.length());
  if(result == ESP_OK){
    Serial.println("" + packet);
  } else {
    Serial.println("Error sending packet");
  }

  delay(30);  // سرعت ارسال
}
