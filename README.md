# IoT – RTC & Indeklima

Dette projekt demonstrerer et simpelt **Real Time Clock**, **temperatur- og luftfugtighedsmåling** samt **menu-styret OLED-display**.  
Projektet er udviklet som en del af EUX Data- og Kommunikationsuddannelsen H3.

---

## Funktioner

- Visning af **tid (HH:MM:SS)** med to cifre (fx `07:06:05`)
- Visning af **dato (DD-MM-YYYY)**
- Måling af:
  - Temperatur (°C)
  - Luftfugtighed (%)
- Menu til at skifte mellem:
  - Tid / Dato
  - Temperatur / Luftfugtighed
  - Indstil tid og dato
- Indstilling af tid og dato med **3 fysiske knapper**
- Ingen eksterne modstande (Arduino **INPUT_PULLUP**)

---

## Betjening (Knapper)

Alle knapper er tilsluttet mellem **pin og GND** og bruger Arduinoens indbyggede pull-up modstand.

| Funktion | Pin | Handling |
|--------|-----|---------|
| **NEXT / MENU** | D6 | Skift skærm / Næste felt |
| **PLUS (+)** | D5 | Øg værdi |
| **MINUS / SAVE** | D3 | Minus / Hold = Gem |

### Indstil tid og dato
1. Gå til **“Indstil tid/dato”** i menuen  
2. Hold **MINUS (D3)** nede i ca. 1,5 sek. for at gå i *EDIT mode*  
3. **NEXT (D6)** → Skift felt (år, måned, dag, time, minut)  
4. **PLUS (D5)** → Øg værdi  
5. **MINUS (D3)** → Mindsk værdi  
6. Hold **MINUS (D3)** igen → **Gem og afslut**

---

## Hardware

### Komponenter
- Arduino Zero
- OLED display (SSD1306, I2C)
- DS3231 Real Time Clock
- DHT11 temperatur- og fugtighedssensor
- 3 trykknapper

### Forbindelser

#### I2C (OLED + RTC)
- SDA → SDA
- SCL → SCL
- VCC → 3.3V
- GND → GND

#### DHT11
- DATA → 7
- VCC → 3.3V
- GND → GND

#### Knapper
- 6 → Knap → GND
- 5 → Knap → GND
- 3 → Knap → GND

---

## Projektstruktur







