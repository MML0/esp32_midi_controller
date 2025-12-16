// ======= Pin Definitions =======
const int pushPins[]   = {2, 4, 5, 12};
const int togPins[]    = {13, 14, 15, 16};
const int analogPins[] = {32, 33, 34, 35, 36, 39};

const int numPush   = sizeof(pushPins) / sizeof(pushPins[0]);
const int numTog    = sizeof(togPins)  / sizeof(togPins[0]);
const int numAnalog = sizeof(analogPins) / sizeof(analogPins[0]);

// ======= Toggle State =======
int togState[4]     = {0, 0, 0, 0};
int togPrevRead[4] = {1, 1, 1, 1};

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  initPins();
}

// ================= LOOP =================
void loop() {
  updateToggles();
  String packet = buildPacket();
  sendPacket(packet);
  delay(30);
}

// ================= FUNCTIONS =================

// ---- Pin init ----
void initPins() {
  for (int i = 0; i < numPush; i++) pinMode(pushPins[i], INPUT_PULLUP);
  for (int i = 0; i < numTog;  i++) pinMode(togPins[i],  INPUT_PULLUP);
}

// ---- Software toggle logic ----
void updateToggles() {
  for (int i = 0; i < numTog; i++) {
    int raw = digitalRead(togPins[i]);

    if (raw == LOW && togPrevRead[i] == HIGH) {
      togState[i] = !togState[i];
    }

    togPrevRead[i] = raw;
  }
}

// ---- Packet builder ----
String buildPacket() {
  String p = "";

  // Push buttons
  for (int i = 0; i < numPush; i++) {
    p += digitalRead(pushPins[i]);
    p += ",";
  }

  // Toggle states
  for (int i = 0; i < numTog; i++) {
    p += togState[i];
    p += ",";
  }

  // Analog values
  for (int i = 0; i < numAnalog; i++) {
    p += analogRead(analogPins[i]);
    if (i < numAnalog - 1) p += ",";
  }

  return p;
}

// ---- Output layer (easy to swap later) ----
void sendPacket(const String& packet) {
  Serial.println(packet);

  // 🔁 Later:
  // esp_now_send(peerAddr, (uint8_t*)packet.c_str(), packet.length());
}
