#pragma once
#include "HT_SSD1306Wire.h"

static SSD1306Wire _disp(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

void VextON()  { pinMode(Vext, OUTPUT); digitalWrite(Vext, LOW);  }
void VextOFF() { pinMode(Vext, OUTPUT); digitalWrite(Vext, HIGH); }

class GradMonDisplay {
  String _name, _set, _cardNum, _grade, _price;
  bool   _dirty = false;

  static String trunc(const String& s, uint8_t n) {
    return s.length() <= n ? s : s.substring(0, n - 1) + "~";
  }

public:
  void begin() {
    VextON();
    delay(100);
    _disp.init();
    _disp.setFont(ArialMT_Plain_10);
  }

  // ── Splash screen ──────────────────────────────────────────────────────────
  void splash() {
    _disp.clear();

    // Cornice esterna
    _disp.drawRect(0, 0, 128, 64);

    // Titolo centrato grande
    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.setFont(ArialMT_Plain_24);
    _disp.drawString(64, 10, "GradMon");

    // Linea decorativa sotto il titolo
    _disp.drawLine(10, 38, 118, 38);

    // Sottotitolo
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64, 44, "Smart Card Display");

    _disp.display();
    delay(2000);
  }

  // ── Schermata generica (stato, messaggi di boot) ───────────────────────────
  void showText(const char* l1, const char* l2 = "",
                const char* l3 = "", const char* l4 = "") {
    _disp.clear();
    _disp.setTextAlignment(TEXT_ALIGN_LEFT);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(0,  0, l1);
    _disp.drawString(0, 14, l2);
    _disp.drawString(0, 28, l3);
    _disp.drawString(0, 42, l4);
    _disp.display();
  }

  // ── AP Setup mode ──────────────────────────────────────────────────────────
  void showAPMode() {
    _disp.clear();
    _disp.drawRect(0, 0, 128, 64);

    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64,  4, "[ SETUP MODE ]");
    _disp.drawLine(4, 17, 124, 17);

    _disp.setTextAlignment(TEXT_ALIGN_LEFT);
    _disp.drawString(4, 20, "WiFi: GradMon-Setup");
    _disp.drawString(4, 34, "Apri il browser:");

    _disp.setFont(ArialMT_Plain_16);
    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.drawString(64, 46, "192.168.4.1");

    _disp.display();
  }

  // ── WiFi connesso ──────────────────────────────────────────────────────────
  void showConnected(const String& ip) {
    _disp.clear();
    _disp.drawRect(0, 0, 128, 64);

    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64,  4, "WIFI CONNESSO");
    _disp.drawLine(4, 17, 124, 17);
    _disp.drawString(64, 20, "Apri il browser:");

    _disp.setFont(ArialMT_Plain_16);
    _disp.drawString(64, 36, ip);

    // Indicatore piccolo
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(64, 54, "GradMon pronto");

    _disp.display();
  }

  // ── Aggiorna carta da mostrare ─────────────────────────────────────────────
  void setCard(const String& name, const String& set,
               const String& cardNum, const String& grade,
               const String& price) {
    _name    = name;
    _set     = set;
    _cardNum = cardNum;
    _grade   = grade;
    _price   = price;
    _dirty   = true;
  }

  // ── Render carta sul display (chiamato da loop) ────────────────────────────
  //
  //  ┌──────────────────────────────┐
  //  │ Charizard Holo Rare          │  Plain_10  y=2
  //  │ Base Set • 4/102             │  Plain_10  y=14
  //  ├══════════════════════════════┤  y=26
  //  │ PSA 10          $ 2,450      │  Plain_16  y=30  (left / right)
  //  │            ● LIVE            │  Plain_10  y=52
  //  └──────────────────────────────┘
  void update() {
    if (!_dirty) return;
    _disp.clear();

    // ── Zona superiore: nome e set ──
    _disp.setTextAlignment(TEXT_ALIGN_LEFT);
    _disp.setFont(ArialMT_Plain_10);
    _disp.drawString(0, 2, trunc(_name, 21));

    // Set + numero carta
    String setLine = trunc(_set, 15);
    if (_cardNum.length()) setLine += " #" + _cardNum;
    _disp.drawString(0, 14, trunc(setLine, 21));

    // ── Separatore doppio ──
    _disp.drawLine(0, 26, 127, 26);
    _disp.drawLine(0, 28, 127, 28);

    // ── Zona inferiore: grade (sinistra) + prezzo (destra) ──
    _disp.setFont(ArialMT_Plain_16);
    _disp.setTextAlignment(TEXT_ALIGN_LEFT);
    _disp.drawString(0, 31, trunc(_grade, 8));

    _disp.setTextAlignment(TEXT_ALIGN_RIGHT);
    _disp.drawString(127, 31, trunc(_price, 9));

    // ── Indicatore LIVE in basso ──
    _disp.setFont(ArialMT_Plain_10);
    _disp.setTextAlignment(TEXT_ALIGN_CENTER);
    _disp.drawString(64, 52, "* LIVE PRICE *");

    _disp.display();
    _dirty = false;
  }
};
