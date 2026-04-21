#pragma once
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

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

  // Costruisce URL immagine da tcgid (es. "ex16-6" → pokemontcg.io/ex16/6.png)
  static String imageFromTcgId(const char* tcgid) {
    if (!tcgid || strlen(tcgid) == 0) return "";
    String t(tcgid);
    int dash = t.lastIndexOf('-');
    if (dash < 0) return "";
    String setId = t.substring(0, dash);
    String num   = t.substring(dash + 1);
    return "https://images.pokemontcg.io/" + setId + "/" + num + ".png";
  }

  // Aggiunge un'opzione prezzo all'array se il valore non è null/zero
  static void addPriceOpt(JsonArray& arr, const char* key, const char* label,
                          const char* group, JsonVariant val, const char* currency) {
    if (val.isNull() || val.as<float>() <= 0) return;
    JsonObject o = arr.createNestedObject();
    o["key"]      = key;
    o["label"]    = label;
    o["group"]    = group;
    o["value"]    = val.as<float>();
    o["currency"] = currency;
  }

  // Estrae tutte le opzioni prezzo disponibili da un oggetto carta
  static void buildPriceOptions(JsonArray& opts, JsonObject prices) {
    // ── Cardmarket ────────────────────────────────────────────────────────────
    if (prices.containsKey("cardmarket")) {
      JsonObject cm = prices["cardmarket"].as<JsonObject>();
      const char* cur = cm["currency"] | "EUR";
      addPriceOpt(opts, "cm_nm",   "Near Mint",    "Cardmarket", cm["lowest_near_mint"],    cur);
      addPriceOpt(opts, "cm_30d",  "Media 30gg",   "Cardmarket", cm["30d_average"],         cur);
      addPriceOpt(opts, "cm_7d",   "Media 7gg",    "Cardmarket", cm["7d_average"],          cur);
      addPriceOpt(opts, "cm_nm_de","NM (DE)",      "Cardmarket", cm["lowest_near_mint_DE"], cur);
      addPriceOpt(opts, "cm_nm_fr","NM (FR)",      "Cardmarket", cm["lowest_near_mint_FR"], cur);
    }
    // ── Graded ────────────────────────────────────────────────────────────────
    if (prices.containsKey("graded")) {
      JsonObject graded = prices["graded"].as<JsonObject>();
      const char* grades[] = {"10","9_5","9","8_5","8","7","6","5","4","3","2","1"};
      const char* labels[] = {"10","9.5","9","8.5","8","7","6","5","4","3","2","1"};
      const char* graders[] = {"psa","bgs","cgc","sgc"};
      const char* graderLabels[] = {"PSA","BGS","CGC","SGC"};

      for (int gi = 0; gi < 4; gi++) {
        if (!graded.containsKey(graders[gi])) continue;
        JsonObject gr = graded[graders[gi]].as<JsonObject>();
        for (int vi = 0; vi < 12; vi++) {
          String key = String(graders[gi]) + grades[vi];   // "psa10", "bgs9_5"
          String lbl = String(graderLabels[gi]) + " " + labels[vi]; // "PSA 10"
          addPriceOpt(opts, key.c_str(), lbl.c_str(), graderLabels[gi],
                      gr[key.c_str()], "EUR");
        }
      }
    }
    // ── TCGPlayer (USD) ───────────────────────────────────────────────────────
    if (prices.containsKey("tcgplayer")) {
      JsonObject tcp = prices["tcgplayer"].as<JsonObject>();
      addPriceOpt(opts, "tcp_market", "Market Price", "TCGPlayer", tcp["market"],     "USD");
      addPriceOpt(opts, "tcp_mid",    "Mid Price",    "TCGPlayer", tcp["mid"],        "USD");
      addPriceOpt(opts, "tcp_low",    "Low Price",    "TCGPlayer", tcp["low"],        "USD");
      addPriceOpt(opts, "tcp_high",   "High Price",   "TCGPlayer", tcp["high"],       "USD");
    }
  }

  // Naviga il JSON di una carta e restituisce il prezzo per la chiave salvata
  static float extractByKey(JsonObject prices, const String& key) {
    if (key.startsWith("cm_")) {
      JsonObject cm = prices["cardmarket"].as<JsonObject>();
      if (key == "cm_nm")    return cm["lowest_near_mint"]    | 0.0f;
      if (key == "cm_30d")   return cm["30d_average"]         | 0.0f;
      if (key == "cm_7d")    return cm["7d_average"]          | 0.0f;
      if (key == "cm_nm_de") return cm["lowest_near_mint_DE"] | 0.0f;
      if (key == "cm_nm_fr") return cm["lowest_near_mint_FR"] | 0.0f;
    }
    if (key.startsWith("tcp_")) {
      JsonObject tcp = prices["tcgplayer"].as<JsonObject>();
      if (key == "tcp_market") return tcp["market"] | 0.0f;
      if (key == "tcp_mid")    return tcp["mid"]    | 0.0f;
      if (key == "tcp_low")    return tcp["low"]    | 0.0f;
    }
    // Graded: key = "psa10", "bgs9_5" — grader = primi 3 char
    String grader = key.substring(0, 3);
    JsonObject gr = prices["graded"][grader.c_str()].as<JsonObject>();
    return gr[key.c_str()] | 0.0f;
  }

public:
  void setApiKey(const String& k) { _apiKey = k; }
  bool ready() const              { return _apiKey.length() > 0; }

  // ── Ricerca carte ─────────────────────────────────────────────────────────
  bool searchCards(const String& query, String& outJson, String& err) {
    if (!ready()) { err = "API key mancante"; return false; }

    String enc = query; enc.replace(" ", "%20");
    String raw;
    if (!_get("/pokemon/cards/search?search=" + enc + "&sort=relevance", raw, err)) return false;

    Serial.println("[API] " + raw.substring(0, 800));

    DynamicJsonDocument in(32768);
    if (deserializeJson(in, raw) != DeserializationError::Ok) {
      err = "JSON parse error"; return false;
    }

    JsonArray src;
    if      (in.is<JsonArray>())            src = in.as<JsonArray>();
    else if (in["data"].is<JsonArray>())    src = in["data"].as<JsonArray>();
    else if (in["cards"].is<JsonArray>())   src = in["cards"].as<JsonArray>();
    else if (in["results"].is<JsonArray>()) src = in["results"].as<JsonArray>();

    DynamicJsonDocument out(20480);
    out["ok"] = true;
    JsonArray arr = out.createNestedArray("results");

    int n = 0;
    for (JsonObject card : src) {
      if (n >= 10) break;
      JsonObject o = arr.createNestedObject();

      // ID come stringa
      if (card["id"].is<int>())
        o["id"] = String(card["id"].as<int>());
      else
        o["id"] = card["id"] | "";

      o["name"]       = card["name"]        | card["card_name"] | "";
      o["nameNum"]    = card["name_numbered"]| "";
      o["cardNumber"] = card["card_number"].is<int>()
                          ? String(card["card_number"].as<int>())
                          : (card["card_number"] | card["number"] | "");
      o["rarity"]     = card["rarity"]      | "";
      o["hp"]         = card["hp"]          | 0;
      o["supertype"]  = card["supertype"]   | "";
      o["tcgid"]      = card["tcgid"]       | "";

      // Set name: prova vari campi
      const char* sn = card["set"]["name"] | card["episode"]["name"]
                     | card["expansion"]   | card["set"]         | "";
      o["setName"] = sn;

      // Immagine: campo diretto oppure costruita da tcgid
      const char* img = card["image"]     | card["image_url"]
                      | card["imageUrl"]  | card["images"]["large"]
                      | card["images"]["small"] | "";
      if (strlen(img) > 0)
        o["imageUrl"] = img;
      else
        o["imageUrl"] = imageFromTcgId(card["tcgid"] | "");

      // Opzioni prezzo disponibili
      JsonArray opts = o.createNestedArray("priceOptions");
      if (card.containsKey("prices"))
        buildPriceOptions(opts, card["prices"].as<JsonObject>());

      n++;
    }

    serializeJson(out, outJson);
    return true;
  }

  // ── Refresh prezzo ogni 24h ───────────────────────────────────────────────
  // priceKey = chiave salvata es. "cm_nm", "psa10"
  bool refreshPrice(const String& cardId, const String& priceKey,
                    float& price, String& err) {
    if (!ready()) { err = "API key mancante"; return false; }
    String raw;
    if (!_get("/pokemon/cards/" + cardId, raw, err)) return false;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, raw) != DeserializationError::Ok) {
      err = "JSON parse error"; return false;
    }
    JsonObject card = doc.is<JsonObject>() ? doc.as<JsonObject>()
                                           : doc["data"].as<JsonObject>();
    if (!card.containsKey("prices")) { err = "Nessun prezzo"; return false; }

    price = extractByKey(card["prices"].as<JsonObject>(), priceKey);
    if (price <= 0) { err = "Prezzo non disponibile"; return false; }
    return true;
  }
};
