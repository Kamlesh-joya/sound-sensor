#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>

/* ========= PIN ========= */
#define SDA_PIN     21
#define SCL_PIN     22
#define SOUND_PIN  34
#define LED1_PIN   25
#define LED2_PIN   26

/* ========= LCD ========= */
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* ========= WIFI ========= */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ========= WEB ========= */
WebServer server(80);

/* ========= VAR ========= */
bool otaEnabled = false;
unsigned long lastClapTime = 0;
int clapState = 0;              // 0=OFF, 1=LED1, 2=LED2
int soundThreshold = 1800;
String sensorStatus = "ALL OFF";

/* ========= WEB PAGE ========= */
void handleRoot() {
  String html = "<h2>ESP32 PRACTICAL STATUS</h2>";
  html += "<p><b>IP:</b> " + WiFi.localIP().toString() + "</p>";
  html += "<p><b>WiFi:</b> ";
  html += (WiFi.status() == WL_CONNECTED) ? "Connected</p>" : "Disconnected</p>";
  html += "<p><b>OTA:</b> ";
  html += otaEnabled ? "ON</p>" : "OFF</p>";
  html += "<p><b>Practical:</b> SOUND</p>";
  html += "<p><b>Sensor:</b> " + sensorStatus + "</p>";

  server.send(200, "text/html", html);
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  /* ---- WIFI ---- */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  lcd.setCursor(0,0);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  /* ---- OTA ---- */
  ArduinoOTA.setHostname("ESP32-SOUND");
  ArduinoOTA.begin();
  otaEnabled = true;

  /* ---- WEB ---- */
  server.on("/", handleRoot);
  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  detectClap();
  updateLCD();
}

/* ================= CLAP LOGIC ================= */
void detectClap() {
  int soundValue = analogRead(SOUND_PIN);

  if (soundValue > soundThreshold && millis() - lastClapTime > 800) {
    lastClapTime = millis();
    clapState++;
    if (clapState > 2) clapState = 0;

    updateLEDs();
  }
}

/* ================= LED UPDATE ================= */
void updateLEDs() {
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  if (clapState == 1) {
    digitalWrite(LED1_PIN, HIGH);
    sensorStatus = "LED1 ON";
  }
  else if (clapState == 2) {
    digitalWrite(LED2_PIN, HIGH);
    sensorStatus = "LED2 ON";
  }
  else {
    sensorStatus = "ALL OFF";
  }
}

/* ================= LCD UPDATE ================= */
void updateLCD() {
  static unsigned long lastLCD = 0;
  if (millis() - lastLCD < 1000) return;
  lastLCD = millis();

  lcd.clear();

  // Row 1: IP
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  // Row 2: OTA + WiFi
  lcd.setCursor(0,1);
  lcd.print("OTA:");
  lcd.print(otaEnabled ? "ON " : "OFF");
  lcd.print(" WiFi:");
  lcd.print(WiFi.status() == WL_CONNECTED ? "ON" : "OFF");

  // Row 3: Practical Name
  lcd.setCursor(0,2);
  lcd.print("Practical:SOUND");

  // Row 4: Sensor Reading
  lcd.setCursor(0,3);
  lcd.print("Sensor:");
  lcd.print(sensorStatus);
}


