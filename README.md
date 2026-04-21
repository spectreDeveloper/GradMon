# GradMon

Case 3D per carte gradate PSA/BGS con display OLED che mostra in tempo reale i prezzi via API TCGgo.

**Hardware:** Heltec ESP32 WiFi Kit 32 V3 (128×64 OLED integrato)

---

## Funzionalità

- Connessione WiFi con setup guidato (AP mode al primo avvio)
- Web UI accessibile da browser su telefono/PC
- Ricerca carte via API [TCGgo](https://www.tcggo.com/api-docs/v1/)
- Selezione casa di gradazione (PSA / BGS / SGC / CGC) e voto
- Prezzo in tempo reale nella valuta preferita
- Preview live sul display OLED
- Persistenza delle impostazioni (riavvio sicuro)

---

## Setup rapido

### 1. Librerie Arduino (Library Manager)

| Libreria | Autore |
|---|---|
| U8g2 | olikraus |
| ArduinoJson | Benoit Blanchon |

### 2. Board support

Aggiungi questo URL in **File → Preferences → Additional Boards Manager URLs**:

```
https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
```

Poi: **Tools → Board → Heltec ESP32 Series → WiFi Kit 32(V3)**

### 3. Flash del firmware

```
Apri firmware/GradMon/GradMon.ino in Arduino IDE
Seleziona la porta COM corretta
Upload
```

---

## Primo avvio

1. Il display mostra **Setup Mode**
2. Connettiti al WiFi **`GradMon-Setup`** (rete aperta)
3. Apri il browser su `192.168.4.1`
4. Inserisci le credenziali WiFi → il dispositivo si riavvia
5. Il display mostra l'IP assegnato dal tuo router
6. Apri quell'IP nel browser e inserisci la tua **API key TCGgo**

---

## Utilizzo

1. **Configura** l'API key e la valuta preferita
2. **Cerca** una carta (nome, set, anno…)
3. Seleziona la carta dai risultati (con anteprima immagine)
4. Scegli **casa di gradazione** e **voto**
5. Premi **Ottieni Prezzo**
6. Premi **Invia al Display** → l'OLED si aggiorna istantaneamente

---

## Pinout OLED (Heltec ESP32 WiFi Kit 32 V3)

| Funzione | GPIO |
|---|---|
| SDA | 17 |
| SCL | 18 |
| RST | 21 |

---

## Sicurezza

- La API key è salvata nella **flash NVS** del chip, mai nel codice sorgente
- Il browser non riceve mai la API key: tutte le chiamate TCGgo passano attraverso l'ESP32
- Le credenziali WiFi sono anch'esse salvate solo in NVS
- Non committare mai file con segreti — vedi `.gitignore`

---

## Adattare gli endpoint TCGgo

Le chiamate API sono in `firmware/GradMon/ApiClient.h`. Verifica i path esatti contro la documentazione ufficiale:

- **Ricerca carta:** `GET /cards/search?q=...`
- **Prezzo:** `GET /cards/{id}/prices?grader=...&grade=...&currency=...`
- **Autenticazione:** `Authorization: Bearer <api_key>`

Adatta i nomi dei campi JSON nei commenti `// Adjust field names` se necessario.

---

## Licenza

MIT
