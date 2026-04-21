# 💎 GradMon
**Smart Display for Graded Cards (PSA/BGS)**

GradMon is a sleek, modern stand for your graded trading cards. It features an integrated OLED display that shows real-time market prices, historical averages, and card details by connecting to the TCGgo API.

---

## ✨ Features
- 🚀 **WiFi Guided Setup**: Easy initial configuration via Access Point mode.
- 📱 **Mobile-Friendly Web UI**: Search and select cards directly from your phone.
- 📈 **Real-Time Data**: Get prices for PSA, BGS, CGC, and SGC from [TCGgo](https://www.tcggo.com/).
- 🖼️ **Image Processing**: Automatically converts card art to sharp, high-contrast black & white using the **Atkinson Dithering** algorithm.
- 🎨 **Multiple Layouts**: Choose from 5 different display modes (Classic, Big Price, Minimal, Card Art, or Price List).
- 💾 **Smart Persistence**: All settings, layouts, and data are saved in the device's memory.

---

## 🛠️ Hardware Requirements
- **Microcontroller**: [Heltec WiFi Kit 32 V3](https://heltec.org/project/wifi-kit-32-v3/) (Integrated 0.96" OLED)
- **Power**: USB-C Cable

---

## 🚀 Getting Started

### 1. Prepare Arduino IDE
1. Install **Arduino IDE**.
2. Add Heltec Board Support:
   - Go to `File → Preferences`.
   - Add this URL to "Additional Boards Manager URLs":
     `https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json`
   - Go to `Tools → Board → Boards Manager`, search for "Heltec ESP32" and install.
3. Install required libraries via Library Manager:
   - `ArduinoJson` (v7+)
   - `Heltec ESP32 Dev-Boards` (by Heltec Automation)

### 2. Flash Firmware
1. Open `firmware/GradMon/GradMon.ino`.
2. Connect your Heltec module via USB.
3. Select `Tools → Board → Heltec ESP32 Series → WiFi Kit 32(V3)`.
4. Click **Upload**.

---

## ⚙️ Initial Configuration

1. **WiFi Setup**: When you first turn it on, look for a WiFi network named `GradMon-Setup`. Connect to it and go to `192.168.4.1` in your browser to save your home WiFi credentials.
2. **API Key**: Once connected to your home WiFi, the display will show an IP address. Open that IP in your browser.
3. **TCGgo API**: Get a free API Key from RapidAPI (Cardmarket TCG API) and enter it in the configuration section of the GradMon web interface.

---

## 📐 3D Case
You can find the **OpenSCAD** model in the `cad/` directory. It is designed to be:
- ✅ **Support-free**: Easy to print on any FDM printer.
- ✅ **Universal**: Fits both PSA and Beckett (BGS) slabs.
- ✅ **Minimal**: Integrated window for the OLED and a rear slot for the USB cable.

---

## 🛡️ Security
- Your API key and WiFi credentials are encrypted and stored in the **ESP32 NVS (Non-Volatile Storage)**.
- Sensitive data is never sent back to the browser once configured.

---

## 📄 License
MIT © [SpectreDeveloper](https://github.com/spectreDeveloper)
