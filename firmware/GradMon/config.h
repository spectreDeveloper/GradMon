#pragma once

// Pin OLED (SDA_OLED, SCL_OLED, RST_OLED, Vext) vengono automaticamente
// dal board "WiFi Kit 32(V3)" — NON ridefinirli qui.

// ── WiFi AP (setup mode) ──────────────────────────────────────────────────────
#define AP_SSID "GradMon-Setup"
#define AP_IP   "192.168.4.1"

// ── Cardmarket TCG API (RapidAPI) ─────────────────────────────────────────────
#define TCGGO_API_HOST "cardmarket-api-tcg.p.rapidapi.com"
#define TCGGO_BASE_URL "https://cardmarket-api-tcg.p.rapidapi.com"

// ── Timing ───────────────────────────────────────────────────────────────────
#define WIFI_TIMEOUT_MS 10000
