// ======= Pin Definitions =======
const int pushPins[]   = {2, 4, 5, 12};          
const int togPins[]    = {13, 14, 15, 16};       
const int analogPins[] = {32, 33, 34, 35, 36, 39};

const int numPush   = sizeof(pushPins) / sizeof(pushPins[0]);
const int numTog    = sizeof(togPins)  / sizeof(togPins[0]);
const int numAnalog = sizeof(analogPins) / sizeof(analogPins[0]);

// ======= Software toggle states (stored) =======
int togState[4] = {0,0,0,0};         // actual toggle values
int togPrevRead[4] = {1,1,1,1};      // previous raw pin read for edge detect


void setup() {
  Serial.begin(115200);

  for(int i=0; i<numPush; i++) pinMode(pushPins[i], INPUT_PULLUP);
  for(int i=0; i<numTog;  i++) pinMode(togPins[i],  INPUT_PULLUP);
}

// ======= Loop =======
void loop() {

  // ====== SOFTWARE TOGGLE LOGIC ======
  for(int i = 0; i < numTog; i++){
    int raw = digitalRead(togPins[i]);

    // detect falling edge (button press)
    if(raw == LOW && togPrevRead[i] == HIGH){
      togState[i] = !togState[i];    // flip 0→1→0→1
    }

    togPrevRead[i] = raw; // update state
  }


  // ====== BUILD PACKET ======
  String packet = "[";

  // Push buttons (normal momentary)
  for(int i=0; i<numPush; i++){
    packet += digitalRead(pushPins[i]);
    packet += ",";
  }

  // Software toggles (state values)
  for(int i=0; i<numTog; i++){
    packet += togState[i];
    packet += ",";
  }

  // Analog reads
  for(int i=0; i<numAnalog; i++){
    packet += analogRead(analogPins[i]);
    if(i < numAnalog-1) packet += ",";
  }

  packet += "]";

  Serial.println(packet);

  delay(30);  // fast & smooth
}
