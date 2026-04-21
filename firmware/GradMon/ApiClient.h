#pragma once
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

// ── ApiClient ─────────────────────────────────────────────────────────────────
// Tutte le chiamate TCGgo passano dall'ESP32 → l'API key non raggiunge mai
// il browser.
//
// Autenticazione: X-RapidAPI-Key + X-RapidAPI-Host (RapidAPI platform)
// Endpoint usati:
//   GET /cards/search?q=name:{query}   → cerca carte
// La risposta include già prices.graded.{psa|bgs|cgc} e prices.cardmarket/tcg_player
class ApiClient {
  String _apiKey;

  bool _get(const String& path, String& body, String& err) {
    WiFiClientSecure client;
    client.setInsecure(); // per produzione: aggiungere CA cert
    HTTPClient http;
    String url = String(TCGGO_BASE_URL) + path;
    http.begin(client, url);
    http.addHeader("X-RapidAPI-Key",  _apiKey);
    http.addHeader("X-RapidAPI-Host", TCGGO_API_HOST);
    http.addHeader("Accept", "application/json");
    http.setTimeout(8000);

    int code = http.GET();
    if (code == 200) {
      body = http.getString();
      http.end();
      return true;
    }
    err = "HTTP " + String(code);
    http.end();
    return false;
  }

public:
  void   setApiKey(const String& k) { _apiKey = k; }
  bool   ready() const              { return _apiKey.length() > 0; }

  // Cerca carte e costruisce la risposta JSON per il browser.
  // Restituisce fino a 10 risultati con name, set, card_number, image e prices.
  bool searchCards(const String& query, String& outJson, String& err) {
    if (!ready()) { err = "API key mancante"; return false; }

    String encoded = query;
    encoded.replace(" ", "%20");
    String path = "/cards/search?q=name:" + encoded;

    String raw;
    if (!_get(path, raw, err)) return false;

    // Documento input: può essere grande (molte carte con prezzi)
    DynamicJsonDocument in(24576);
    if (deserializeJson(in, raw) != DeserializationError::Ok) {
      err = "JSON parse error";
      return false;
    }

    // Prendiamo l'array "data" o direttamente l'array root
    JsonArray src = in.is<JsonArray>() ? in.as<JsonArray>()
                                       : in["data"].as<JsonArray>();

    // Documento output ridotto: solo i campi necessari
    DynamicJsonDocument out(16384);
    out["ok"] = true;
    JsonArray arr = out.createNestedArray("results");

    int count = 0;
    for (JsonObject card : src) {
      if (count >= 10) break;
      JsonObject o = arr.createNestedObject();
      o["id"]         = card["id"];
      o["name"]       = card["name"];
      o["setName"]    = card["episode"]["name"];
      o["cardNumber"] = card["card_number"];
      o["imageUrl"]   = card["image"];

      // Copia il sotto-oggetto prices (graded + market)
      JsonObject prices = o.createNestedObject("prices");
      if (card.containsKey("prices")) {
        JsonObject src_p = card["prices"].as<JsonObject>();

        // Prezzi ungraded
        if (src_p.containsKey("tcg_player"))
          prices["usd"] = src_p["tcg_player"]["market_price"];
        if (src_p.containsKey("cardmarket"))
          prices["eur"] = src_p["cardmarket"]["market_price"];

        // Prezzi graded: psa, bgs, cgc, sgc
        if (src_p.containsKey("graded")) {
          JsonObject graded    = prices.createNestedObject("graded");
          JsonObject src_graded = src_p["graded"].as<JsonObject>();
          for (JsonPair kv : src_graded) {
            graded[kv.key()] = kv.value();
          }
        }
      }
      count++;
    }

    serializeJson(out, outJson);
    return true;
  }
};
