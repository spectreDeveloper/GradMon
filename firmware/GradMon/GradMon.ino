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

// ── Refresh automatico ogni 24h ───────────────────────────────────────────────
#define REFRESH_INTERVAL_MS (24UL * 60UL * 60UL * 1000UL)
unsigned long lastRefreshMs = 0;
bool          firstRefreshDone = false;

// Simbolo valuta per formattare il prezzo sull'OLED
String currencySymbol(const String& c) {
  if (c == "EUR") return "€";
  if (c == "GBP") return "£";
  if (c == "JPY") return "¥";
  return "$";
}

String formatPrice(float val, const String& currency) {
  char buf[16];
  // Nessun decimale se cifra tonda, altrimenti 2
  if ((long)val == val) snprintf(buf, sizeof(buf), "%.0f", val);
  else                  snprintf(buf, sizeof(buf), "%.2f", val);
  return currencySymbol(currency) + String(buf);
}

void doRefresh() {
  if (!storage.hasRefreshData()) return;
  if (!api.ready())              return;
  if (WiFi.status() != WL_CONNECTED) return;

  String cardId    = storage.getRefreshCardId();
  String graderKey = storage.getRefreshGraderKey();
  String currency  = storage.getCurrency();

  Serial.println("[refresh] cardId=" + cardId + " key=" + graderKey);
  oled.showText("Aggiornamento...", storage.getCardName().c_str());

  float price; String err;
  if (api.refreshPrice(cardId, graderKey, currency, price, err)) {
    String formatted = formatPrice(price, currency);
    String name  = storage.getCardName();
    String set   = storage.getCardSet();
    String num   = storage.getRefreshCardNum();
    String grade = storage.getCardGrade();

    oled.setCard(name, set, num, grade, formatted);
    storage.saveCard(name, set, grade, formatted);
    lastRefreshMs = millis();
    Serial.println("[refresh] OK: " + formatted);
  } else {
    // Mostra ugualmente i dati vecchi
    oled.setCard(storage.getCardName(), storage.getCardSet(),
                 storage.getRefreshCardNum(),
                 storage.getCardGrade(), storage.getCardPrice());
    Serial.println("[refresh] ERR: " + err);
  }
  firstRefreshDone = true;
}

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  oled.begin();
  oled.splash();

  storage.begin();

  if (storage.hasApiKey()) api.setApiKey(storage.getApiKey());

  // Connessione WiFi
  String ssid = storage.getSSID();
  String pass = storage.getPass();

  if (ssid.length()) {
    oled.showText("Connessione WiFi...", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < WIFI_TIMEOUT_MS)
      delay(200);
  }

  if (WiFi.status() == WL_CONNECTED) {
    oled.showConnected(WiFi.localIP().toString());
    server.begin(false);
    Serial.println("IP: " + WiFi.localIP().toString());
    delay(1500);
    // Refresh immediato al boot se c'è una carta salvata
    doRefresh();
    lastRefreshMs = millis();
  } else {
    // Nessun WiFi → AP setup mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID);
    oled.showAPMode();
    server.begin(true);
    Serial.println("AP mode: " AP_SSID);
  }

  // Mostra la carta salvata se non è stato fatto il refresh
  if (!firstRefreshDone && storage.getCardName().length()) {
    oled.setCard(storage.getCardName(), storage.getCardSet(),
                 storage.getRefreshCardNum(),
                 storage.getCardGrade(), storage.getCardPrice());
  }
}

void loop() {
  server.handle();
  oled.update();

  // Refresh automatico ogni 24h
  if (WiFi.status() == WL_CONNECTED &&
      millis() - lastRefreshMs >= REFRESH_INTERVAL_MS) {
    doRefresh();
  }
}
