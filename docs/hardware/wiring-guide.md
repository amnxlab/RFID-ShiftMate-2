# RFID-Shift-Mate 2 - Hardware Wiring Guide

## Overview

This document provides the complete pin mapping for connecting all hardware components to the ESP32-WROOM-32.

**ESP32 Board:** ESP32-WROOM-32 DevKit v1 (38-pin or 30-pin variant)

---

## Component List

1. **ESP32-WROOM-32** Development Board
2. **MFRC522** RFID Reader Module
3. **W5500** Ethernet Module (SPI version with RJ45 jack)
4. **ST7789V** 2.0" TFT Display (240x320, SPI)
5. **DS3231** RTC Module
6. **Passive Buzzer** (3.3V or 5V)
7. **Status LED** (or use onboard LED)
8. **3x Push Buttons** (momentary, normally open)
9. **PoE PD Controller** (e.g., Si3402 module or integrated on W5500 board)

---

## SPI Bus Architecture

The ESP32 has **two SPI buses**:

### VSPI (Default: GPIO 5, 18, 19, 23)
Used by: **MFRC522 RFID Reader**

### HSPI (Remapped: GPIO 13, 14, 15, 16, 17, 32, 33)
Used by: **W5500 Ethernet** + **ST7789V Display** (shared bus, separate CS pins)

---

## Complete Pin Mapping Table

| Component | Pin Label | ESP32 GPIO | Notes |
|-----------|-----------|------------|-------|
| **VSPI Bus (RFID)** | | | |
| MFRC522 | MOSI | **GPIO 23** | VSPI MOSI (hardware) |
| MFRC522 | MISO | **GPIO 19** | VSPI MISO (hardware) |
| MFRC522 | SCK | **GPIO 18** | VSPI SCK (hardware) |
| MFRC522 | SS/SDA | **GPIO 5** | VSPI Chip Select |
| MFRC522 | RST | **GPIO 4** | Reset pin |
| MFRC522 | VCC | **3.3V** | Power supply |
| MFRC522 | GND | **GND** | Ground |
| | | | |
| **I2C Bus (RTC)** | | | |
| DS3231 | SDA | **GPIO 21** | I2C Data |
| DS3231 | SCL | **GPIO 22** | I2C Clock |
| DS3231 | VCC | **3.3V** | Power supply |
| DS3231 | GND | **GND** | Ground |
| | | | |
| **HSPI Bus (Ethernet + Display)** | | | |
| W5500 | MOSI | **GPIO 13** | HSPI MOSI (shared with display) |
| W5500 | MISO | **GPIO 14** | HSPI MISO (shared with display) |
| W5500 | SCK | **GPIO 15** | HSPI SCK (shared with display) |
| W5500 | CS/SS | **GPIO 16** | Ethernet chip select |
| W5500 | RST | **GPIO 17** | Ethernet reset |
| W5500 | INT | **Not connected** | Using polling mode |
| W5500 | VCC | **3.3V** | Power supply |
| W5500 | GND | **GND** | Ground |
| | | | |
| ST7789V | MOSI/SDA | **GPIO 13** | HSPI MOSI (shared with W5500) |
| ST7789V | SCK/SCL | **GPIO 15** | HSPI SCK (shared with W5500) |
| ST7789V | CS | **GPIO 32** | Display chip select |
| ST7789V | DC/RS | **GPIO 33** | Data/Command select |
| ST7789V | RST | **GPIO 25** | Display reset |
| ST7789V | BL/LED | **GPIO 26** | Backlight control (PWM) |
| ST7789V | VCC | **3.3V** | Power supply |
| ST7789V | GND | **GND** | Ground |
| | | | |
| **Buttons** | | | |
| Button UP | One side | **GPIO 34** | Input-only pin, use internal pull-up |
| Button UP | Other side | **GND** | Ground |
| Button DOWN | One side | **GPIO 35** | Input-only pin, use internal pull-up |
| Button DOWN | Other side | **GND** | Ground |
| Button SELECT | One side | **GPIO 36** | Input-only pin, use internal pull-up |
| Button SELECT | Other side | **GND** | Ground |
| | | | |
| **Audio/Visual** | | | |
| Buzzer (+) | Anode/Positive | **GPIO 27** | PWM output |
| Buzzer (-) | Cathode/Negative | **GND** | Ground |
| | | | |
| Status LED (+) | Anode | **GPIO 2** | Onboard LED (blinks during boot) |
| Status LED (-) | Cathode | **GND** | Via 330Ω resistor |
| | | | |
| **PoE Power Input** | | | |
| PoE Module | VIN+ | **PoE+ 48V** | From PoE switch/injector |
| PoE Module | VIN- | **GND** | PoE ground |
| PoE Module | VOUT+ | **5V** | Regulated 5V output |
| PoE Module | VOUT- | **GND** | Power ground |

---

## Critical Wiring Notes

### 1. GPIO 15 (HSPI SCK) - Boot Strapping

**GPIO 15 must be HIGH at boot** for normal ESP32 operation.

**Solution:** Add a **10kΩ pull-up resistor** from GPIO 15 to 3.3V.

```
3.3V ──[10kΩ]── GPIO 15 (HSPI SCK)
```

This ensures GPIO 15 is HIGH during boot, and the SPI peripheral can drive it LOW during operation.

### 2. GPIO 2 (Status LED) - Boot Strapping

**GPIO 2 must be LOW at boot** for normal ESP32 operation.

The onboard LED will blink during boot, which is normal. If you add an external LED:
- Use a 330Ω resistor in series
- The LED will blink during boot, then behave as configured

### 3. SPI Bus Sharing (HSPI)

The W5500 and ST7789V **share the same SPI bus** (GPIO 13, 14, 15) but have **separate CS pins**:
- W5500 CS: GPIO 16
- Display CS: GPIO 32

**Only one device should be active at a time.** The firmware handles this by:
- Setting W5500 CS LOW when communicating with Ethernet
- Setting Display CS LOW when updating the display
- Keeping the other CS HIGH (inactive)

### 4. Button Wiring

Buttons use **input-only pins** (GPIO 34, 35, 36) which **cannot use internal pull-ups**.

**Solution:** Add **10kΩ pull-up resistors** for each button.

```
3.3V ──[10kΩ]── GPIO 34 ──[Button]── GND
```

Or use the ESP32's internal pull-ups if the pins support it (GPIO 34-39 do NOT have internal pull-ups, so external resistors are required).

### 5. Power Supply

**Total estimated current draw:**
- ESP32: ~240mA (WiFi active)
- W5500: ~120mA
- Display: ~60mA
- RFID: ~14mA
- RTC: ~0.1mA
- LED: ~10mA
- Buzzer: ~30mA (when active)

**Total:** ~474mA typical, ~500mA peak

**PoE provides:** Up to 12.95W at 48V = ~270mA at 48V

**Power path:**
```
PoE Switch (48V) → PoE PD Module → 5V DC → ESP32 Vin or 5V pin
                                          → 3.3V regulator (AMS1117) → 3.3V rail
```

**Important:** 
- Use the PoE module's **regulated 5V output** to power the ESP32
- The ESP32's onboard 3.3V regulator can supply ~500mA, which is sufficient
- Add 100µF and 0.1µF decoupling capacitors near the ESP32 power pins

---

## Wiring Diagrams

### SPI Bus Layout

```
ESP32
 ├── VSPI Bus (RFID Only)
 │   ├── GPIO 23 → RFID MOSI
 │   ├── GPIO 19 ← RFID MISO
 │   ├── GPIO 18 → RFID SCK
 │   ├── GPIO 5  → RFID CS
 │   └── GPIO 4  → RFID RST
 │
 └── HSPI Bus (W5500 + Display Shared)
     ├── GPIO 13 → W5500 MOSI & Display MOSI (shared)
     ├── GPIO 14 ← W5500 MISO & Display MISO (shared)
     ├── GPIO 15 → W5500 SCK & Display SCK (shared, with 10k pullup)
     ├── GPIO 16 → W5500 CS (Ethernet)
     ├── GPIO 17 → W5500 RST
     ├── GPIO 32 → Display CS
     ├── GPIO 33 → Display DC
     ├── GPIO 25 → Display RST
     └── GPIO 26 → Display BL
```

### I2C Bus Layout

```
ESP32
 └── I2C Bus (RTC)
     ├── GPIO 21 → DS3231 SDA
     └── GPIO 22 → DS3231 SCL
```

### Button Layout

```
3.3V
 ├── [10kΩ] ── GPIO 34 ── [UP Button] ── GND
 ├── [10kΩ] ── GPIO 35 ── [DOWN Button] ── GND
 └── [10kΩ] ── GPIO 36 ── [SELECT Button] ── GND
```

### Power Layout

```
PoE Switch (48V DC)
    ↓
PoE PD Controller Module
    ↓
5V DC Output
    ↓
    ├── ESP32 Vin pin (or 5V pin)
    │       ↓
    │   ESP32 internal 3.3V regulator
    │       ↓
    │   3.3V rail (powers all modules)
    │
    └── Decoupling capacitors
        ├── 100µF electrolytic (near ESP32)
        └── 0.1µF ceramic (near ESP32)
```

---

## Required Additional Components

### Resistors

| Value | Quantity | Purpose |
|-------|----------|---------|
| 10kΩ | 4 | Pull-ups for GPIO 15 (SCK) and 3x buttons |
| 330Ω | 1 | Current limiting for status LED (if not using onboard) |

### Capacitors

| Value | Quantity | Purpose |
|-------|----------|---------|
| 100µF | 1 | Power supply decoupling (bulk) |
| 0.1µF | 1 | Power supply decoupling (HF) |

### PoE Components

| Component | Example Part | Notes |
|-----------|--------------|-------|
| PoE PD Controller | Si3402-B-GM or module | 48V to 5V isolated converter |
| PoE Switch | 8-port 802.3af/at | 120W total budget |
| OR PoE Injector | 48V 15W injector | If switch doesn't have PoE |

---

## Assembly Checklist

Before powering on:

- [ ] All GND connections are common (ESP32 GND = all module GNDs)
- [ ] GPIO 15 has 10kΩ pull-up to 3.3V
- [ ] All 3 buttons have 10kΩ pull-ups to 3.3V
- [ ] W5500 and Display share MOSI, MISO, SCK but have separate CS pins
- [ ] PoE module outputs regulated 5V (not raw 48V)
- [ ] Decoupling capacitors are near ESP32 power pins
- [ ] No short circuits between pins
- [ ] All modules powered from 3.3V (except PoE input)

---

## Power-On Sequence

1. **Connect PoE cable** from PoE switch to W5500 RJ45 jack
2. **Verify PoE module output** is 5V (measure with multimeter)
3. **Power on ESP32** (via PoE module 5V output)
4. **Observe boot sequence:**
   - GPIO 2 LED blinks (normal boot)
   - Display shows "RFID ShiftMate 2" boot screen
   - Ethernet link LED on W5500 lights up
5. **Check serial monitor** (115200 baud) for initialization messages

---

## Troubleshooting

### Display not working
- Check CS pin (GPIO 32) is connected
- Verify 3.3V power to display
- Check SPI pins (13, 14, 15) are connected

### Ethernet not connecting
- Check W5500 CS pin (GPIO 16) is connected
- Verify RJ45 cable is plugged into PoE switch
- Check PoE module is providing power

### RFID not reading cards
- Check RFID CS pin (GPIO 5) is connected
- Verify 3.3V power to RFID module
- Check SPI pins (23, 19, 18) are connected

### Buttons not responding
- Verify 10kΩ pull-up resistors are installed
- Check button wiring (one side to GPIO, other to GND)
- Test with multimeter (should show 3.3V when not pressed, 0V when pressed)

### ESP32 not booting
- Check GPIO 15 has 10kΩ pull-up to 3.3V
- Verify power supply is stable 5V
- Check for short circuits

---

## Component Purchasing List

| Component | Recommended Part | Quantity | Est. Cost |
|-----------|------------------|----------|-----------|
| ESP32 DevKit | ESP32-WROOM-32 DevKit v1 | 1 | $6-10 |
| RFID Reader | MFRC522 module | 1 | $3-5 |
| Ethernet Module | W5500 module with RJ45 | 1 | $8-12 |
| Display | ST7789V 2.0" TFT SPI | 1 | $8-12 |
| RTC Module | DS3231 module | 1 | $3-5 |
| PoE PD Module | Si3402 or similar | 1 | $10-15 |
| PoE Switch | 8-port 802.3af/at | 1 | $40-60 |
| Passive Buzzer | 3.3V or 5V | 1 | $1-2 |
| Push Buttons | 6x6mm momentary | 3 | $1 |
| Resistors | 10kΩ, 330Ω | 5 | $1 |
| Capacitors | 100µF, 0.1µF | 2 | $1 |
| **Total** | | | **$82-123** |

---

## Next Steps

After wiring:

1. **Flash firmware** using PlatformIO
2. **Test each component** individually via serial monitor
3. **Verify Ethernet connection** and IP assignment
4. **Test RFID reading** with test cards
5. **Verify display** shows correct UI
6. **Test portal communication** (once portal API is implemented)

---

**Document Version:** 1.0  
**Date:** 2026-11-05  
**Status:** Ready for Assembly
