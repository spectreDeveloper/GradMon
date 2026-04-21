/*
 * GradMon — Heltec ESP32 WiFi Kit 32 V3
 *
 * Libraries required (Arduino Library Manager):
 *   - Heltec ESP32 Dev-Boards  by Heltec Automation
 *   - ArduinoJson              by Benoit Blanchon (v7)
 *
 * Board support:
 *   File → Preferences → Additional boards URL:
 *   https://resource.heltec.cn/download/package_heltec_esp32_index.json
 *   Tools → Board → Heltec ESP32 Series → WiFi Kit 32(V3)
 */

#include <WiFi.h>
#include "config.h"
#include "Storage.h"
#include "GradMonDisplay.h"
#include "ApiClient.h"
#include "GradMonServer.h"

Storage        storage;
GradMonDisplay oled;
ApiClient      api;
GradMonServer  server(&storage, &api, &oled);

bool apMode = false;

void setup() {
  Serial.begin(115200);
  oled.begin();   // abilita Vext GPIO36 LOW, poi inizializza SSD1306
  oled.splash();

  storage.begin();

  // Restore API key and saved card from flash
  if (storage.hasApiKey()) {
    api.setApiKey(storage.getApiKey());
  }
  if (storage.getCardName().length()) {
    oled.setCard(storage.getCardName(), storage.getCardSet(),
                 "", storage.getCardGrade(), storage.getCardPrice());
  }

  // Try to connect to saved WiFi
  String ssid = storage.getSSID();
  String pass = storage.getPass();

  if (ssid.length()) {
    oled.showText("Connessione WiFi...", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
      delay(200);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    oled.showConnected(WiFi.localIP().toString());
    server.begin(false);
    Serial.println("IP: " + WiFi.localIP().toString());
  } else {
    // No credentials or connection failed → start AP setup mode
    apMode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    oled.showAPMode();
    server.begin(true);
    Serial.println("AP mode: " AP_SSID);
  }
}

void loop() {
  server.handle();
  oled.update();
}
