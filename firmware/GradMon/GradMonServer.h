#pragma once
#include <WebServer.h>
#include <ArduinoJson.h>
#include "Storage.h"
#include "ApiClient.h"
#include "GradMonDisplay.h"
#include "HtmlContent.h"

class GradMonServer {
  WebServer       _srv;
  Storage*        _storage;
  ApiClient*      _api;
  GradMonDisplay* _display;
  bool            _apMode;

  void jsonReply(int code, const String& body) {
    _srv.sendHeader("Access-Control-Allow-Origin", "*");
    _srv.send(code, "application/json", body);
  }
  void ok(const String& extra = "") {
    jsonReply(200, "{\"ok\":true" + (extra.length() ? "," + extra : "") + "}");
  }
  void err(const String& msg, int code = 400) {
    jsonReply(code, "{\"ok\":false,\"error\":\"" + msg + "\"}");
  }

  // ── AP mode ───────────────────────────────────────────────────────────────
  void handleWifiPage() { _srv.send_P(200, "text/html", HTML_WIFI_SETUP); }

  void handleSaveWifi() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, _srv.arg("plain"))) { err("JSON invalido"); return; }
    String ssid = doc["ssid"] | "";
    String pass = doc["pass"] | "";
    if (!ssid.length()) { err("SSID mancante"); return; }
    _storage->saveWifi(ssid, pass);
    ok();
    delay(500);
    ESP.restart();
  }

  // ── Normal mode ───────────────────────────────────────────────────────────
  void handleIndex() { _srv.send_P(200, "text/html", HTML_INDEX); }

  void handleConfig() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, _srv.arg("plain"))) { err("JSON invalido"); return; }
    String key      = doc["apiKey"]   | "";
    String currency = doc["currency"] | "USD";
    if (!key.length()) { err("API key mancante"); return; }
    _storage->saveApiKey(key);
    _storage->saveCurrency(currency);
    _api->setApiKey(key);
    ok();
  }

  // Ricerca carte — risposta include prices embedded, il browser estrae il prezzo
  void handleSearch() {
    String q = _srv.arg("q");
    if (!q.length()) { err("Parametro q mancante"); return; }
    if (!_api->ready()) { err("API key non configurata"); return; }

    String outJson, errMsg;
    if (!_api->searchCards(q, outJson, errMsg)) { err(errMsg); return; }
    jsonReply(200, outJson);
  }

  // Riceve nome, set, cardNumber, grade, price già formattato dal browser
  void handleSetDisplay() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, _srv.arg("plain"))) { err("JSON invalido"); return; }
    String name     = doc["name"]       | "";
    String setName  = doc["setName"]    | "";
    String cardNum  = doc["cardNumber"] | "";
    String grade    = doc["grade"]      | "";
    String price    = doc["price"]      | "";
    if (!name.length()) { err("name mancante"); return; }

    _display->setCard(name, setName, cardNum, grade, price);
    _storage->saveCard(name, setName, grade, price);
    ok();
  }

  void handleStatus() {
    DynamicJsonDocument out(512);
    out["ok"]        = true;
    out["apiReady"]  = _api->ready();
    out["currency"]  = _storage->getCurrency();
    out["cardName"]  = _storage->getCardName();
    out["cardSet"]   = _storage->getCardSet();
    out["cardGrade"] = _storage->getCardGrade();
    out["cardPrice"] = _storage->getCardPrice();
    String body; serializeJson(out, body);
    jsonReply(200, body);
  }

public:
  GradMonServer(Storage* s, ApiClient* a, GradMonDisplay* d)
    : _srv(80), _storage(s), _api(a), _display(d), _apMode(false) {}

  void begin(bool apMode = false) {
    _apMode = apMode;
    if (apMode) {
      _srv.on("/",         HTTP_GET,  [this]{ handleWifiPage(); });
      _srv.on("/api/wifi", HTTP_POST, [this]{ handleSaveWifi(); });
    } else {
      _srv.on("/",            HTTP_GET,  [this]{ handleIndex();      });
      _srv.on("/api/config",  HTTP_POST, [this]{ handleConfig();     });
      _srv.on("/api/search",  HTTP_GET,  [this]{ handleSearch();     });
      _srv.on("/api/display", HTTP_POST, [this]{ handleSetDisplay(); });
      _srv.on("/api/status",  HTTP_GET,  [this]{ handleStatus();     });
    }
    _srv.onNotFound([this]{ _srv.send(404, "text/plain", "Not found"); });
    _srv.begin();
  }

  void handle() { _srv.handleClient(); }
};
