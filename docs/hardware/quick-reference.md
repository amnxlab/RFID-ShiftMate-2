# RFID-Shift-Mate 2 - Quick Reference Pin Map

## Visual Pin Layout

```
┌─────────────────────────────────────────────────────────────┐
│                      ESP32-WROOM-32                          │
│                                                              │
│  GPIO 34 ◄── BTN UP          VCC 3.3V ──┬── RFID VCC       │
│  GPIO 35 ◄── BTN DOWN        VCC 3.3V ──┼── W5500 VCC      │
│  GPIO 36 ◄── BTN SELECT      VCC 3.3V ──┼── Display VCC    │
│  GPIO 27 ─── BUZZER          VCC 3.3V ──┘── DS3231 VCC     │
│  GPIO 2  ─── LED                                                              │
│                                                              │
│  VSPI Bus (RFID Only)         I2C Bus (RTC)                 │
│  ┌──────────────┐              ┌──────────────┐             │
│  │ GPIO 23 → MOSI│              │ GPIO 21 → SDA│             │
│  │ GPIO 19 ← MISO│              │ GPIO 22 → SCL│             │
│  │ GPIO 18 → SCK │              └──────────────┘             │
│  │ GPIO 5  → CS  │                                          │
│  │ GPIO 4  → RST │              HSPI Bus (W5500+Display)    │
│  └──────────────┘              ┌──────────────────────┐     │
│                                 │ GPIO 13 → MOSI       │     │
│                                 │ GPIO 14 ← MISO       │     │
│                                 │ GPIO 15 → SCK  [10k] │     │
│                                 │ GPIO 16 → W5500 CS   │     │
│                                 │ GPIO 17 → W5500 RST  │     │
│                                 │ GPIO 32 → Display CS │     │
│                                 │ GPIO 33 → Display DC │     │
│                                 │ GPIO 25 → Display RST│     │
│                                 │ GPIO 26 → Display BL │     │
│                                 └──────────────────────┘     │
│                                                              │
│  GND ──────────────────────────────────────────────────      │
└─────────────────────────────────────────────────────────────┘

[10k] = 10kΩ pull-up resistor to 3.3V required
```

---

## Component Connections Summary

### 1. MFRC522 RFID Reader (VSPI)
```
MFRC522    →    ESP32
─────────────────────
MOSI       →    GPIO 23
MISO       →    GPIO 19
SCK        →    GPIO 18
SS/SDA     →    GPIO 5
RST        →    GPIO 4
VCC        →    3.3V
GND        →    GND
```

### 2. W5500 Ethernet Module (HSPI)
```
W5500      →    ESP32
─────────────────────
MOSI       →    GPIO 13
MISO       →    GPIO 14
SCK        →    GPIO 15  [add 10kΩ pullup to 3.3V]
CS/SS      →    GPIO 16
RST        →    GPIO 17
INT        →    Not connected
VCC        →    3.3V
GND        →    GND
```

### 3. ST7789V 2.0" TFT Display (HSPI)
```
ST7789V    →    ESP32
─────────────────────
MOSI/SDA   →    GPIO 13  (shared with W5500)
SCK/SCL    →    GPIO 15  (shared with W5500)
CS         →    GPIO 32
DC/RS      →    GPIO 33
RST        →    GPIO 25
BL/LED     →    GPIO 26
VCC        →    3.3V
GND        →    GND
```

### 4. DS3231 RTC Module (I2C)
```
DS3231     →    ESP32
─────────────────────
SDA        →    GPIO 21
SCL        →    GPIO 22
VCC        →    3.3V
GND        →    GND
```

### 5. Buttons (3x)
```
Button     →    ESP32
─────────────────────
UP         →    GPIO 34  [add 10kΩ pullup to 3.3V]
DOWN       →    GPIO 35  [add 10kΩ pullup to 3.3V]
SELECT     →    GPIO 36  [add 10kΩ pullup to 3.3V]

Other side of all buttons → GND
```

### 6. Buzzer & LED
```
Buzzer (+) →    GPIO 27
Buzzer (-) →    GND

LED (+)    →    GPIO 2  [via 330Ω resistor]
LED (-)    →    GND
```

### 7. Power (USB/DC - Same as Original)
```
Power Source → ESP32 DevKit
─────────────────────────────────
USB Cable  →    Micro-USB port
OR
DC Jack    →    Vin pin + GND (7-12V DC)
```

---

## Required Resistors

| Location | Value | Purpose |
|----------|-------|---------|
| GPIO 15 → 3.3V | 10kΩ | Boot strapping pull-up |
| GPIO 34 → 3.3V | 10kΩ | Button UP pull-up |
| GPIO 35 → 3.3V | 10kΩ | Button DOWN pull-up |
| GPIO 36 → 3.3V | 10kΩ | Button SELECT pull-up |
| GPIO 2 → LED | 330Ω | LED current limiting |

---

## Critical Checks Before Power-On

✅ GPIO 15 has 10kΩ pull-up to 3.3V  
✅ All 3 buttons have 10kΩ pull-ups to 3.3V  
✅ W5500 and Display share MOSI/MISO/SCK but have separate CS  
✅ All GND connections are common  
✅ No short circuits between pins  
✅ Decoupling capacitors installed near ESP32  

---

## Power Requirements

- **Input:** 5V via USB or 7-12V via DC jack (same as original RFID-Shift-Mate)
- **3.3V Rail:** ESP32 onboard regulator
- **Total Current:** ~500mA peak
- **Total Power:** ~1.8W peak

---

## Quick Test After Wiring

1. Power on via USB cable
2. Connect Ethernet cable from W5500 to your network switch/router
3. Open serial monitor at 115200 baud
4. You should see:
   ```
   RFID-Shift-Mate 2
   Initializing...
   Ethernet connected
   Display ready
   RFID reader ready
   ```
5. Display should show boot screen
6. W5500 Ethernet LED should be lit

---

**Full Details:** See [docs/hardware/wiring-guide.md](docs/hardware/wiring-guide.md)
