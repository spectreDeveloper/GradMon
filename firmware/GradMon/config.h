#pragma once

// ── OLED Display Pins (Heltec ESP32 WiFi Kit 32 V3) ──────────────────────────
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21

// ── WiFi AP (setup mode, no password → open network) ─────────────────────────
#define AP_SSID "GradMon-Setup"
#define AP_IP   "192.168.4.1"

// ── TCGgo API ─────────────────────────────────────────────────────────────────
// Verify the exact base URL against https://www.tcggo.com/api-docs/v1/
#define TCGGO_BASE_URL "https://api.tcggo.com/v1"

// ── Timing ───────────────────────────────────────────────────────────────────
#define WIFI_TIMEOUT_MS   10000
#define PRICE_REFRESH_MS  60000   // auto-refresh price every 60 s
