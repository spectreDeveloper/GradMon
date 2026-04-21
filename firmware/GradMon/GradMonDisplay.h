#pragma once
#include "HT_SSD1306Wire.h"

static SSD1306Wire _disp(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

// Bitmap personalizzato per il simbolo Euro (8x10 px)
const uint8_t PROGMEM euro_bitmap[] = {
  0x38, 0x44, 0x40, 0xfc, 0x40, 0xfc, 0x40, 0x44, 0x38, 0x00
};

void VextON()  { pinMode(Vext, OUTPUT); digitalWrite(Vext, LOW);  }
void VextOFF() { pinMode(Vext, OUTPUT); digitalWrite(Vext, HIGH); }

class GradMonDisplay {
  String _name, _set, _cardNum, _grade, _price;
  String _p2, _p3, _p4;
  uint8_t _image[280]; // 40x56 bits = 280 bytes
  bool    _hasImage = false;
  uint8_t _layout = 0;
  bool    _dirty = false;

  static String trunc(const String& s, uint8_t n) {
    return s.length() <= n ? s : s.substring(0, n - 1) + "~";
  }

  // Disegna il simbolo dell'Euro usando un bitmap hardware (8x10)
  void drawEuro(int x, int y, bool large = false) {
    // SSD1306Wire non ha drawBitmap diretto per array piccoli in questo modo, 
    // usiamo drawXbm o disegniamo pixel per pixel per massima compatibilità.
    // L'Euro è 8px wide.
    for (int j = 0; j < 10; j++) {
      uint8_t b = pgm_read_byte(&euro_bitmap[j]);
      for (int i = 0; i < 8; i++) {
        if (b & (0x80 >> i)) {
          if (large) {
            _disp.setPixel(x + i*1.2, y + j*1.2); // Scaling brutale ma efficace
            _disp.setPixel(x + i*1.2 + 1, y + j*1.2);
          } else {
            _disp.setPixel(x + i, y + j);
          }
        }
      }
    }
  }

  void drawPrice(int x, int y, String s, bool large = false, bool right = false) {
    String clean = s;
    bool hasEuro = (s.indexOf("€") >= 0 || s.indexOf("\xE2\x82\xAC") >= 0);
    clean.replace("€", ""); clean.replace("\xE2\x82\xAC", "");
    clean.trim();

    int tw = _disp.getStringWidth(clean);
    int symW = 8;
    int totalW = tw + symW + 3;

    int startX = x;
    if (right) startX = x - totalW;
    else if (_disp.getTextAlignment() == TEXT_ALIGN_CENTER) startX = x - (totalW / 2);

    if (hasEuro) {
      drawEuro(startX, y + (large ? 4 : 2), large);
      _disp.setTextAlignment(TEXT_ALIGN_LEFT);
      _disp.drawString(startX + symW + 3, y, clean);
    } else {
      _disp.setTextAlignment(right ? TEXT_ALIGN_RIGHT : (startX == x ? TEXT_ALIGN_LEFT : TEXT_ALIGN_CENTER));
      _disp.drawString(x, y, s);
    }
  }

public:
  void begin() {
    VextON(); delay(100);
    _disp.init();
    _disp.setFont(ArialMT_Plain_10);
  }

  void splash() {
    _disp.clear();
    _disp.drawRect(0, 0, 128, 64);
    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.setFont(ArialMT_Plain_24);
    _disp.drawString(64, 10, "GradMon");
    _disp.drawLine(10, 38, 118, 38);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64, 44, "Smart Card Display");
    _disp.display();
    delay(2000);
  }

  void showText(const char* l1, const char* l2 = "", const char* l3 = "", const char* l4 = "") {
    _disp.clear(); _disp.setTextAlignment(TEXT_ALIGN_LEFT);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(0, 0, l1); _disp.drawString(0, 14, l2);
    _disp.drawString(0, 28, l3); _disp.drawString(0, 42, l4);
    _disp.display();
  }

  void showAPMode() {
    _disp.clear(); _disp.drawRect(0, 0, 128, 64);
    _disp.setTextAlignment(TEXT_ALIGN_CENTER); _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64, 4, "[ SETUP MODE ]"); _disp.drawLine(4, 17, 124, 17);
    _disp.setTextAlignment(TEXT_ALIGN_LEFT); _disp.drawString(4, 20, "WiFi: GradMon-Setup");
    _disp.drawString(4, 34, "Apri il browser:");
    _disp.setFont(ArialMT_Plain_16); _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.drawString(64, 46, "192.168.4.1"); _disp.display();
  }

  void showConnected(const String& ip) {
    _disp.clear(); _disp.drawRect(0, 0, 128, 64);
    _disp.setTextAlignment(TEXT_ALIGN_CENTER); _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64, 4, "WIFI CONNESSO"); _disp.drawLine(4, 17, 124, 17);
    _disp.drawString(64, 20, "Apri il browser:");
    _disp.setFont(ArialMT_Plain_16); _disp.drawString(64, 36, ip);
    _disp.setFont(ArialMT_Plain_10); _disp.drawString(64, 54, "GradMon pronto");
    _disp.display();
  }

  void setCard(const String& name, const String& set, const String& cardNum, const String& grade, const String& price, uint8_t layout = 0, const uint8_t* bmp = nullptr, const String& p2 = "", const String& p3 = "", const String& p4 = "") {
    _name = name; _set = set; _cardNum = cardNum; _grade = grade; _price = price; _layout = layout;
    _p2 = p2; _p3 = p3; _p4 = p4;
    if (bmp) { memcpy(_image, bmp, 280); _hasImage = true; } else { _hasImage = false; }
    _dirty = true;
  }

  void update() {
    if (!_dirty) return;
    _disp.clear();

    if (_layout == 1) { // Prezzo Grande
      _disp.setFont(ArialMT_Plain_10); _disp.setTextAlignment(TEXT_ALIGN_CENTER);
      _disp.drawString(64, 2, trunc(_name, 26)); _disp.drawLine(10, 14, 118, 14);
      _disp.setFont(ArialMT_Plain_24);
      drawPrice(64, 18, trunc(_price, 10), true, false);
      _disp.setFont(ArialMT_Plain_10); _disp.setTextAlignment(TEXT_ALIGN_CENTER);
      _disp.drawString(64, 48, trunc(_grade, 21));
    } 
    else if (_layout == 2) { // Minimal
      _disp.setFont(ArialMT_Plain_16); _disp.setTextAlignment(TEXT_ALIGN_LEFT);
      _disp.drawString(0, 4, trunc(_name, 14)); _disp.drawLine(0, 22, 127, 22);
      _disp.setFont(ArialMT_Plain_24);
      drawPrice(0, 28, trunc(_price, 10), true);
    }
    else if (_layout == 3) { // Immagine + Info
      if (_hasImage) _disp.drawXbm(0, 4, 40, 56, _image);
      else _disp.drawRect(0, 4, 40, 56);
      _disp.setFont(ArialMT_Plain_10); _disp.setTextAlignment(TEXT_ALIGN_LEFT);
      _disp.drawString(44, 4, trunc(_name, 15));
      String sl = trunc(_set, 10); if (_cardNum.length()) sl += " #" + _cardNum;
      _disp.drawString(44, 16, trunc(sl, 15));
      _disp.setFont(ArialMT_Plain_16);
      drawPrice(44, 30, trunc(_price, 9), false);
      _disp.setFont(ArialMT_Plain_10);
      _disp.drawString(44, 48, trunc(_grade, 15));
    }
    else if (_layout == 4) { // Tutti i prezzi
      _disp.setFont(ArialMT_Plain_10); _disp.setTextAlignment(TEXT_ALIGN_CENTER);
      _disp.drawString(64, 0, trunc(_name, 26));
      _disp.drawLine(0, 11, 127, 11);
      _disp.setTextAlignment(TEXT_ALIGN_LEFT);
      if (_price.length()) drawPrice(0, 14, _price);
      if (_p2.length())    drawPrice(0, 26, _p2);
      if (_p3.length())    drawPrice(0, 38, _p3);
      if (_p4.length())    drawPrice(0, 50, _p4);
    }
    else { // Classico
      _disp.setFont(ArialMT_Plain_10); _disp.setTextAlignment(TEXT_ALIGN_LEFT);
      _disp.drawString(0, 2, trunc(_name, 21));
      String sl = trunc(_set, 15); if (_cardNum.length()) sl += " #" + _cardNum;
      _disp.drawString(0, 14, trunc(sl, 21));
      _disp.drawLine(0, 26, 127, 26); _disp.drawLine(0, 28, 127, 28);
      _disp.setFont(ArialMT_Plain_16);
      _disp.drawString(0, 31, trunc(_grade, 8));
      drawPrice(127, 31, trunc(_price, 9), false, true);
    }
    _disp.display(); _dirty = false;
  }
};
