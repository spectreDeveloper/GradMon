#pragma once
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

// ── ApiClient ─────────────────────────────────────────────────────────────────
// Tutte le chiamate TCGgo transitano dall'ESP32 → l'API key non raggiunge mai
// il browser.
//
// Auth: X-RapidAPI-Key + X-RapidAPI-Host
// Endpoint usati:
//   GET /cards/search?q=name:{q}   → ricerca + prezzi embedded
//   GET /cards/{id}                → refresh prezzo carta corrente
class ApiClient {
  String _apiKey;

  bool _get(const String& path, String& body, String& err) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.begin(client, String(TCGGO_BASE_URL) + path);
    http.addHeader("x-rapidapi-key",  _apiKey);
    http.addHeader("x-rapidapi-host", TCGGO_API_HOST);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);
    int code = http.GET();
    if (code == 200) { body = http.getString(); http.end(); return true; }
    err = "HTTP " + String(code);
    http.end();
    return false;
  }

  // Estrae il prezzo dal sotto-oggetto prices di una singola carta
  float _extractPrice(JsonObject card, const String& graderKey,
                      const String& currency) {
    // graderKey = "psa10", "bgs9_5", "cgc10" …
    // grader = primi 3 caratteri minuscoli
    String grader = graderKey.substring(0, 3);

    if (card.containsKey("prices")) {
      JsonObject prices = card["prices"].as<JsonObject>();
      // Prezzo graded
      if (prices.containsKey("graded")) {
        JsonObject gr = prices["graded"].as<JsonObject>();
        if (gr.containsKey(grader.c_str())) {
          JsonVariant v = gr[grader.c_str()][graderKey.c_str()];
          if (!v.isNull()) return v.as<float>();
        }
      }
      // Fallback: prezzo di mercato ungraded
      if (currency == "EUR" && prices.containsKey("cardmarket"))
        return prices["cardmarket"]["market_price"] | 0.0f;
      if (prices.containsKey("tcg_player"))
        return prices["tcg_player"]["market_price"] | 0.0f;
    }
    return 0.0f;
  }

public:
  void setApiKey(const String& k) { _apiKey = k; }
  bool ready() const              { return _apiKey.length() > 0; }

  // ── Ricerca carte ─────────────────────────────────────────────────────────
  // Restituisce JSON già pronto per il browser con prezzi embedded.
  bool searchCards(const String& query, String& outJson, String& err) {
    if (!ready()) { err = "API key mancante"; return false; }

    String enc = query; enc.replace(" ", "%20");
    String raw;
    if (!_get("/pokemon/cards/search?search=" + enc + "&sort=relevance", raw, err)) return false;

    // Log primi 500 char per debug struttura risposta
    Serial.println("[API] response: " + raw.substring(0, 500));

    DynamicJsonDocument in(24576);
    if (deserializeJson(in, raw) != DeserializationError::Ok) {
      err = "JSON parse error"; return false;
    }
    // Supporta: array root, {"data":[...]}, {"cards":[...]}, {"results":[...]}
    JsonArray src;
    if      (in.is<JsonArray>())              src = in.as<JsonArray>();
    else if (in["data"].is<JsonArray>())      src = in["data"].as<JsonArray>();
    else if (in["cards"].is<JsonArray>())     src = in["cards"].as<JsonArray>();
    else if (in["results"].is<JsonArray>())   src = in["results"].as<JsonArray>();

    DynamicJsonDocument out(16384);
    out["ok"] = true;
    JsonArray arr = out.createNestedArray("results");

    int n = 0;
    for (JsonObject card : src) {
      if (n >= 10) break;
      JsonObject o = arr.createNestedObject();
      // Campi con fallback per nomi alternativi
      o["id"]         = card["id"]        | card["card_id"]  | "";
      o["name"]       = card["name"]      | card["card_name"]| "";
      // Set: prova episode.name, set.name, expansion, set
      const char* sn = card["episode"]["name"] | card["set"]["name"]
                     | card["expansion"]       | card["set"] | "";
      o["setName"]    = sn;
      o["cardNumber"] = card["card_number"] | card["number"] | card["cardNumber"] | "";
      o["imageUrl"]   = card["image"]       | card["image_url"] | card["imageUrl"] | "";

      JsonObject prices = o.createNestedObject("prices");
      if (card.containsKey("prices")) {
        JsonObject sp = card["prices"].as<JsonObject>();
        if (sp.containsKey("tcg_player"))
          prices["usd"] = sp["tcg_player"]["market_price"];
        if (sp.containsKey("cardmarket"))
          prices["eur"] = sp["cardmarket"]["market_price"];
        if (sp.containsKey("graded")) {
          JsonObject graded = prices.createNestedObject("graded");
          for (JsonPair kv : sp["graded"].as<JsonObject>())
            graded[kv.key()] = kv.value();
        }
      }
      n++;
    }

    serializeJson(out, outJson);
    return true;
  }

  // ── Refresh prezzo carta corrente (chiamato ogni 24h dall'ESP) ────────────
  // graderKey: es. "psa10", "bgs9_5"
  // price (out): prezzo aggiornato
  bool refreshPrice(const String& cardId, const String& graderKey,
                    const String& currency, float& price, String& err) {
    if (!ready()) { err = "API key mancante"; return false; }

    String raw;
    if (!_get("/cards/" + cardId, raw, err)) return false;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, raw) != DeserializationError::Ok) {
      err = "JSON parse error"; return false;
    }

    // La risposta può essere l'oggetto carta direttamente o {"data":{...}}
    JsonObject card = doc.is<JsonObject>() ? doc.as<JsonObject>()
                                           : doc["data"].as<JsonObject>();

    price = _extractPrice(card, graderKey, currency);
    if (price <= 0) { err = "Prezzo non disponibile"; return false; }
    return true;
  }
};
