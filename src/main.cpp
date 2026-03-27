#define BLYNK_TEMPLATE_ID "TMPL_DEMO_RELAY"
#define BLYNK_TEMPLATE_NAME "CollegeDemoRelay"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

enum ControlMode {
  MODE_CLOUD = 0,
  MODE_OFFLINE = 1
};

// -------- User settings --------
const char BLYNK_AUTH_TOKEN[] = "Yb3z84yZvG5RrDlzF1oPYfkQM7dHRGdM";
const char WIFI_SSID[] = "DPS_SG";
const char WIFI_PASS[] = "DPSCampus@48EG";

const uint8_t RELAY_PIN = D1;      // Relay 1
const bool RELAY_ACTIVE_LOW = true;
const uint8_t BLYNK_RELAY_VPIN = V0;
const unsigned long CLOUD_RETRY_MS = 7000;
// -------------------------------

ControlMode currentMode = MODE_CLOUD;
bool relayIsOn = false;
unsigned long lastCloudRetryMs = 0;
String serialBuffer;

void applyRelayState(bool turnOn) {
  relayIsOn = turnOn;
  const uint8_t relayOutput = RELAY_ACTIVE_LOW
                                  ? (turnOn ? LOW : HIGH)
                                  : (turnOn ? HIGH : LOW);
  digitalWrite(RELAY_PIN, relayOutput);
  Serial.printf("[RELAY] %s\n", relayIsOn ? "ON" : "OFF");
}

const char *modeToText(ControlMode mode) {
  return (mode == MODE_CLOUD) ? "CLOUD" : "OFFLINE";
}

void printStatus() {
  Serial.printf("[STATUS] mode=%s relay=%s wifi=%s blynk=%s\n",
                modeToText(currentMode),
                relayIsOn ? "ON" : "OFF",
                (WiFi.status() == WL_CONNECTED) ? "CONNECTED" : "DISCONNECTED",
                Blynk.connected() ? "CONNECTED" : "DISCONNECTED");
}

void setMode(ControlMode newMode) {
  if (currentMode == newMode) {
    Serial.printf("[MODE] Already in %s mode\n", modeToText(currentMode));
    return;
  }

  currentMode = newMode;
  Serial.printf("[MODE] Switched to %s mode\n", modeToText(currentMode));

  if (currentMode == MODE_OFFLINE) {
    if (Blynk.connected()) {
      Blynk.disconnect();
    }
  } else {
    lastCloudRetryMs = 0;
  }
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.length() == 0) {
    return;
  }

  if (cmd == "MODE OFFLINE") {
    setMode(MODE_OFFLINE);
  } else if (cmd == "MODE CLOUD") {
    setMode(MODE_CLOUD);
  } else if (cmd == "RELAY ON") {
    if (currentMode != MODE_OFFLINE) {
      Serial.println("[BLOCKED] Serial relay control disabled in CLOUD mode");
      return;
    }
    applyRelayState(true);
    if (Blynk.connected()) {
      Blynk.virtualWrite(BLYNK_RELAY_VPIN, 1);
    }
  } else if (cmd == "RELAY OFF") {
    if (currentMode != MODE_OFFLINE) {
      Serial.println("[BLOCKED] Serial relay control disabled in CLOUD mode");
      return;
    }
    applyRelayState(false);
    if (Blynk.connected()) {
      Blynk.virtualWrite(BLYNK_RELAY_VPIN, 0);
    }
  } else if (cmd == "STATUS") {
    printStatus();
  } else if (cmd == "HELP") {
    Serial.println("[HELP] MODE OFFLINE | MODE CLOUD | RELAY ON | RELAY OFF | STATUS");
  } else {
    Serial.printf("[ERROR] Unknown command: %s\n", cmd.c_str());
  }
}

void readSerialCommands() {
  while (Serial.available() > 0) {
    const char incoming = static_cast<char>(Serial.read());
    if (incoming == '\n' || incoming == '\r') {
      if (!serialBuffer.isEmpty()) {
        processCommand(serialBuffer);
        serialBuffer = "";
      }
    } else {
      serialBuffer += incoming;
    }
  }
}

void ensureCloudConnection() {
  if (currentMode != MODE_CLOUD) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (Blynk.connected()) {
    Blynk.run();
    return;
  }

  const unsigned long now = millis();
  if (now - lastCloudRetryMs < CLOUD_RETRY_MS) {
    return;
  }
  lastCloudRetryMs = now;

  Serial.println("[CLOUD] Connecting to Blynk...");
  const bool connected = Blynk.connect(2000);
  Serial.printf("[CLOUD] Blynk %s\n", connected ? "connected" : "not connected");
}

BLYNK_WRITE(V0) {
  if (currentMode != MODE_CLOUD) {
    Serial.println("[CLOUD] Ignored Blynk relay request (OFFLINE mode)");
    return;
  }

  const bool requestOn = (param.asInt() == 1);
  applyRelayState(requestOn);
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(BLYNK_RELAY_VPIN, relayIsOn ? 1 : 0);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(RELAY_PIN, OUTPUT);
  applyRelayState(false);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Blynk.config(BLYNK_AUTH_TOKEN);

  Serial.println();
  Serial.println("ESP8266 Dual-Mode Relay Demo");
  Serial.println("Commands: MODE OFFLINE | MODE CLOUD | RELAY ON | RELAY OFF | STATUS | HELP");
  printStatus();
}

void loop() {
  readSerialCommands();
  ensureCloudConnection();
}
