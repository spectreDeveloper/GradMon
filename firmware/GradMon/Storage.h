#pragma once
#include <Preferences.h>

class Storage {
  Preferences _prefs;

public:
  void begin() { _prefs.begin("gradmon", false); }

  // ── WiFi ──────────────────────────────────────────────────────────────────
  void saveWifi(const String& ssid, const String& pass) {
    _prefs.putString("ssid", ssid);
    _prefs.putString("pass", pass);
  }
  void   clearWifi() { _prefs.remove("ssid"); _prefs.remove("pass"); }
  String getSSID()   { return _prefs.getString("ssid", ""); }
  String getPass()   { return _prefs.getString("pass", ""); }

  // ── API key (solo in NVS, mai esposta al browser) ─────────────────────────
  void   saveApiKey(const String& k) { _prefs.putString("apikey", k); }
  String getApiKey()  { return _prefs.getString("apikey", ""); }
  bool   hasApiKey()  { return _prefs.getString("apikey", "").length() > 0; }

  // ── Preferenze utente ─────────────────────────────────────────────────────
  void   saveCurrency(const String& c) { _prefs.putString("currency", c); }
  String getCurrency() { return _prefs.getString("currency", "USD"); }

  // ── Carta corrente (persiste ai riavvii) ──────────────────────────────────
  void saveCard(const String& name, const String& set,
                const String& grade, const String& price,
                const String& p2 = "", const String& p3 = "", const String& p4 = "") {
    _prefs.putString("c_name",  name);
    _prefs.putString("c_set",   set);
    _prefs.putString("c_grade", grade);
    _prefs.putString("c_price", price);
    _prefs.putString("c_p2",    p2);
    _prefs.putString("c_p3",    p3);
    _prefs.putString("c_p4",    p4);
  }
  void saveCardImage(const uint8_t* data, size_t len) {
    if (len > 0) _prefs.putBytes("c_img", data, len);
    else _prefs.remove("c_img");
  }
  String getCardName()  { return _prefs.getString("c_name",  ""); }
  String getCardSet()   { return _prefs.getString("c_set",   ""); }
  String getCardGrade() { return _prefs.getString("c_grade", ""); }
  String getCardPrice() { return _prefs.getString("c_price", ""); }
  String getCardP2()    { return _prefs.getString("c_p2",    ""); }
  String getCardP3()    { return _prefs.getString("c_p3",    ""); }
  String getCardP4()    { return _prefs.getString("c_p4",    ""); }
  size_t getCardImage(uint8_t* buf, size_t maxLen) {
    return _prefs.getBytes("c_img", buf, maxLen);
  }
  bool hasCardImage() { return _prefs.isKey("c_img"); }

  // ── Dati per il refresh automatico (cardId + chiave prezzo + cardNumber) ──
  // graderKey = es. "psa10", "bgs9_5" — costruito dal browser e salvato qui
  void saveRefresh(const String& cardId, const String& cardNum,
                   const String& priceKey) {
    _prefs.putString("r_id",    cardId);
    _prefs.putString("r_num",   cardNum);
    _prefs.putString("r_pkey",  priceKey);   // es. "cm_nm", "psa10"
  }
  String getRefreshCardId()   { return _prefs.getString("r_id",   ""); }
  String getRefreshCardNum()  { return _prefs.getString("r_num",  ""); }
  String getRefreshPriceKey() { return _prefs.getString("r_pkey", ""); }
  bool   hasRefreshData()     { return _prefs.getString("r_id",   "").length() > 0; }

  // ── Layout OLED (0-3) ─────────────────────────────────────────────────────
  void saveLayout(int l) { _prefs.putInt("layout", l); }
  int  getLayout()       { return _prefs.getInt("layout", 0); }
};
