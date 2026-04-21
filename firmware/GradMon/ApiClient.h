#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// ── Data types ────────────────────────────────────────────────────────────────
struct CardResult {
  String id;
  String name;
  String setName;
  String year;
  String imageUrl;
};

struct PriceResult {
  float  value   = 0;
  String currency;
  bool   valid   = false;
  String error;
};

// ── ApiClient ─────────────────────────────────────────────────────────────────
// All TCGgo calls are proxied through the ESP32 so the API key is never
// exposed to the browser.
//
// IMPORTANT: Endpoint paths & response field names may need adjustment once
// you verify the exact schema at https://www.tcggo.com/api-docs/v1/
class ApiClient {
  String _apiKey;

  bool _get(const String& url, String& body, String& err) {
    HTTPClient http;
    http.begin(url);
    // NOTE: for production add proper CA cert instead of setInsecure()
    http.setInsecure();
    http.addHeader("Authorization", "Bearer " + _apiKey);
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
  void setApiKey(const String& key) { _apiKey = key; }
  bool ready() const { return _apiKey.length() > 0; }

  // Search cards — fills `results` (up to 10 items)
  bool searchCards(const String& query, std::vector<CardResult>& results, String& err) {
    results.clear();
    if (!ready()) { err = "No API key"; return false; }

    String encoded = query;
    encoded.replace(" ", "%20");
    String url = String(TCGGO_BASE_URL) + "/cards/search?q=" + encoded + "&limit=10";

    String body;
    if (!_get(url, body, err)) return false;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, body) != DeserializationError::Ok) {
      err = "JSON parse error";
      return false;
    }

    // Adjust field names to match actual TCGgo response schema
    JsonArray arr = doc.is<JsonArray>() ? doc.as<JsonArray>()
                                        : doc["data"].as<JsonArray>();
    for (JsonObject item : arr) {
      CardResult r;
      r.id       = item["id"]       | item["card_id"]    | "";
      r.name     = item["name"]     | item["card_name"]  | "";
      r.setName  = item["set"]      | item["set_name"]   | "";
      r.year     = item["year"]     | "";
      r.imageUrl = item["image"]    | item["image_url"]  | "";
      if (r.id.length() && r.name.length()) results.push_back(r);
    }
    return true;
  }

  // Get market price for a specific card + grader + grade
  bool getPrice(const String& cardId, const String& grader,
                const String& grade, const String& currency,
                PriceResult& result) {
    if (!ready()) { result.error = "No API key"; return false; }

    // Adjust path to match actual TCGgo price endpoint
    String url = String(TCGGO_BASE_URL)
               + "/cards/" + cardId + "/prices"
               + "?grader=" + grader
               + "&grade=" + grade
               + "&currency=" + currency;

    String body;
    if (!_get(url, body, result.error)) return false;

    DynamicJsonDocument doc(2048);
    if (deserializeJson(doc, body) != DeserializationError::Ok) {
      result.error = "JSON parse error";
      return false;
    }

    // Adjust field names to match actual TCGgo price response
    result.value    = doc["price"] | doc["market_price"] | doc["value"] | 0.0f;
    result.currency = currency;
    result.valid    = true;
    return true;
  }
};
