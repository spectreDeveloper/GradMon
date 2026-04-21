#pragma once
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"

class GradMonDisplay {
  // SSD1306 128×64 — hardware I2C
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C _u8g2;

  String _line1, _line2, _line3, _line4;
  bool   _dirty = false;

  static String trunc(const String& s, uint8_t maxChars) {
    if (s.length() <= maxChars) return s;
    return s.substring(0, maxChars - 1) + "~";
  }

public:
  GradMonDisplay()
    : _u8g2(U8G2_R0, OLED_RST, OLED_SCL, OLED_SDA) {}

  void begin() {
    Wire.begin(OLED_SDA, OLED_SCL);
    _u8g2.begin();
  }

  void splash() {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_10x20_tf);
    _u8g2.drawStr(14, 28, "GradMon");
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(22, 48, "v1.0  Loading...");
    _u8g2.sendBuffer();
    delay(1800);
  }

  void showText(const char* l1, const char* l2 = "", const char* l3 = "", const char* l4 = "") {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(0,  10, l1);
    _u8g2.drawStr(0,  24, l2);
    _u8g2.drawStr(0,  38, l3);
    _u8g2.drawStr(0,  52, l4);
    _u8g2.sendBuffer();
  }

  void showAPMode() {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(0, 10, "== Setup Mode ==");
    _u8g2.drawStr(0, 24, "WiFi: GradMon-Setup");
    _u8g2.drawStr(0, 38, "Poi apri browser:");
    _u8g2.drawStr(0, 52, AP_IP);
    _u8g2.sendBuffer();
  }

  void showConnected(const String& ip) {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(0, 10, "WiFi connesso!");
    _u8g2.drawStr(0, 24, "Apri il browser:");
    _u8g2.drawStr(0, 38, ip.c_str());
    _u8g2.drawStr(0, 52, "per configurare");
    _u8g2.sendBuffer();
  }

  // Called from web UI when user presses "Invia al display"
  void setCard(const String& name, const String& set,
               const String& grade, const String& price) {
    _line1 = trunc(name,  21);
    _line2 = trunc(set,   21);
    _line3 = trunc(grade, 21);
    _line4 = trunc(price, 21);
    _dirty = true;
  }

  // Call from loop()
  void update() {
    if (!_dirty) return;
    _u8g2.clearBuffer();

    // Two-font layout: small for name/set, larger for grade/price
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(0, 10, _line1.c_str());
    _u8g2.drawStr(0, 22, _line2.c_str());
    _u8g2.drawHLine(0, 26, 128);

    _u8g2.setFont(u8g2_font_7x13_tf);
    _u8g2.drawStr(0, 42, _line3.c_str());
    _u8g2.drawStr(0, 60, _line4.c_str());

    _u8g2.sendBuffer();
    _dirty = false;
  }
};
