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

  // ── helpers ─────────────────────────────────────────────────────────────────
  void json(int code, const String& body) {
    _srv.sendHeader("Access-Control-Allow-Origin", "*");
    _srv.send(code, "application/json", body);
  }

  void ok(const String& extra = "") {
    json(200, "{\"ok\":true" + (extra.length() ? "," + extra : "") + "}");
  }

  void err(const String& msg, int code = 400) {
    json(code, "{\"ok\":false,\"error\":\"" + msg + "\"}");
  }

  String bodyStr() { return _srv.arg("plain"); }

  // ── AP mode handlers ─────────────────────────────────────────────────────────
  void handleWifiPage() {
    _srv.send_P(200, "text/html", HTML_WIFI_SETUP);
  }

  void handleSaveWifi() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, bodyStr())) { err("JSON invalido"); return; }
    String ssid = doc["ssid"] | "";
    String pass = doc["pass"] | "";
    if (!ssid.length()) { err("SSID mancante"); return; }
    _storage->saveWifi(ssid, pass);
    ok();
    delay(500);
    ESP.restart();
  }

  // ── Normal mode handlers ─────────────────────────────────────────────────────
  void handleIndex() {
    _srv.send_P(200, "text/html", HTML_INDEX);
  }

  void handleConfig() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, bodyStr())) { err("JSON invalido"); return; }
    String key      = doc["apiKey"]  | "";
    String currency = doc["currency"] | "USD";
    if (!key.length()) { err("API key mancante"); return; }
    _storage->saveApiKey(key);
    _storage->saveCurrency(currency);
    _api->setApiKey(key);
    ok();
  }

  void handleSearch() {
    String q = _srv.arg("q");
    if (!q.length()) { err("Parametro q mancante"); return; }
    if (!_api->ready()) { err("API key non configurata"); return; }

    std::vector<CardResult> results;
    String errMsg;
    if (!_api->searchCards(q, results, errMsg)) { err(errMsg); return; }

    DynamicJsonDocument out(8192);
    out["ok"] = true;
    JsonArray arr = out.createNestedArray("results");
    for (auto& r : results) {
      JsonObject obj = arr.createNestedObject();
      obj["id"]       = r.id;
      obj["name"]     = r.name;
      obj["setName"]  = r.setName;
      obj["year"]     = r.year;
      obj["imageUrl"] = r.imageUrl;
    }
    String body;
    serializeJson(out, body);
    json(200, body);
  }

  void handlePrice() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, bodyStr())) { err("JSON invalido"); return; }
    String cardId   = doc["cardId"]   | "";
    String grader   = doc["grader"]   | "PSA";
    String grade    = doc["grade"]    | "10";
    String currency = doc["currency"] | _storage->getCurrency();
    if (!cardId.length()) { err("cardId mancante"); return; }
    if (!_api->ready()) { err("API key non configurata"); return; }

    PriceResult pr;
    if (!_api->getPrice(cardId, grader, grade, currency, pr)) { err(pr.error); return; }

    DynamicJsonDocument out(256);
    out["ok"]       = true;
    out["price"]    = pr.value;
    out["currency"] = pr.currency;
    String body;
    serializeJson(out, body);
    json(200, body);
  }

  void handleSetDisplay() {
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, bodyStr())) { err("JSON invalido"); return; }
    String name    = doc["name"]    | "";
    String setName = doc["setName"] | "";
    String grade   = doc["grade"]   | "";
    String price   = doc["price"]   | "";
    if (!name.length()) { err("name mancante"); return; }

    _display->setCard(name, setName, grade, price);
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
    String body;
    serializeJson(out, body);
    json(200, body);
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
      _srv.on("/",           HTTP_GET,  [this]{ handleIndex();      });
      _srv.on("/api/config", HTTP_POST, [this]{ handleConfig();     });
      _srv.on("/api/search", HTTP_GET,  [this]{ handleSearch();     });
      _srv.on("/api/price",  HTTP_POST, [this]{ handlePrice();      });
      _srv.on("/api/display",HTTP_POST, [this]{ handleSetDisplay(); });
      _srv.on("/api/status", HTTP_GET,  [this]{ handleStatus();     });
    }

    _srv.onNotFound([this]{ _srv.send(404, "text/plain", "Not found"); });
    _srv.begin();
  }

  void handle() { _srv.handleClient(); }
};
