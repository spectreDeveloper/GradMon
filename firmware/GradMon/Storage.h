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
  void clearWifi() { _prefs.remove("ssid"); _prefs.remove("pass"); }
  String getSSID() { return _prefs.getString("ssid", ""); }
  String getPass() { return _prefs.getString("pass", ""); }

  // ── API key (stored only in NVS flash, never returned to browser) ──────────
  void saveApiKey(const String& key) { _prefs.putString("apikey", key); }
  String getApiKey()  { return _prefs.getString("apikey", ""); }
  bool   hasApiKey()  { return _prefs.getString("apikey", "").length() > 0; }

  // ── User preferences ───────────────────────────────────────────────────────
  void saveCurrency(const String& c) { _prefs.putString("currency", c); }
  String getCurrency() { return _prefs.getString("currency", "USD"); }

  // ── Currently displayed card (persists across reboots) ────────────────────
  void saveCard(const String& name, const String& set,
                const String& grade, const String& price) {
    _prefs.putString("c_name",  name);
    _prefs.putString("c_set",   set);
    _prefs.putString("c_grade", grade);
    _prefs.putString("c_price", price);
  }
  String getCardName()  { return _prefs.getString("c_name",  ""); }
  String getCardSet()   { return _prefs.getString("c_set",   ""); }
  String getCardGrade() { return _prefs.getString("c_grade", ""); }
  String getCardPrice() { return _prefs.getString("c_price", ""); }
};
