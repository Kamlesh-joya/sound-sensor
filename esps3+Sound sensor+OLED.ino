#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

/* ================== PIN DEFINITIONS ================== */
#define SDA_PIN     8
#define SCL_PIN     9
#define SOUND_PIN   6      // 🎤 Sound sensor analog pin
#define LED_PIN     48     // RGB LED (ESP32-S3)

/* ================== RGB LED ================== */
#define LED_COUNT 1
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ================== LCD ================== */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ================== OLED ================== */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ================== WIFI ================== */
const char* ssid = "Excitel_HARSHIT_2.4G";
const char* password = "dinesh480tak";

/* ================== VARIABLES ================== */
int soundThreshold = 1800;     // clap sensitivity (adjust if needed)
int currentColor = 0;
unsigned long lastClapTime = 0;

/* ================== COLORS ================== */
const char* colorNames[] = {"RED", "GREEN", "BLUE", "PURPLE"};
uint32_t colors[] = {
  led.Color(255, 0, 0),
  led.Color(0, 255, 0),
  led.Color(0, 0, 255),
  led.Color(255, 0, 255)
};

/* ================== FUNCTION PROTOTYPE ================== */
void updateDisplays();

/* ================== SETUP ================== */
void setup() {
  Serial.begin(115200);

  /* I2C */
  Wire.begin(SDA_PIN, SCL_PIN);

  /* LCD INIT */
  lcd.init();
  lcd.backlight();

  /* OLED INIT (FIXED ORIENTATION) */
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  oled.setRotation(2);          // 🔥 OLED ULTA ISSUE FIX
  oled.clearDisplay();

  /* RGB INIT */
  led.begin();
  led.clear();
  led.show();

  /* WIFI */
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  /* OTA */
  ArduinoOTA.setHostname("ESP32-S3");
  ArduinoOTA.begin();

  updateDisplays();
}

/* ================== LOOP ================== */
void loop() {
  ArduinoOTA.handle();   // 🔥 OTA NEVER REMOVE

  int soundValue = analogRead(SOUND_PIN);
  Serial.println(soundValue);

  // Clap detect (debounced)
  if (soundValue > soundThreshold && millis() - lastClapTime > 800) {
    lastClapTime = millis();
    currentColor = (currentColor + 1) % 4;
    updateDisplays();
  }
}

/* ================== DISPLAY + LED UPDATE ================== */
void updateDisplays() {
  /* RGB LED */
  led.clear();
  led.setPixelColor(0, colors[currentColor]);
  led.show();

  /* LCD (NORMAL TEXT) */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sound Trigger");
  lcd.setCursor(0, 1);
  lcd.print("Color: ");
  lcd.print(colorNames[currentColor]);

  /* OLED (BIG TEXT + BOX) */
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(10, 10);
  oled.println(colorNames[currentColor]);

  oled.drawRect(20, 40, 88, 18, SSD1306_WHITE);
  oled.fillRect(22, 42, 84, 14, SSD1306_WHITE);

  oled.display();
}


