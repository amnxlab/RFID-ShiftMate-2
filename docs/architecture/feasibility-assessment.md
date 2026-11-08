# RFID-Shift-Mate 2 - Technical Feasibility Assessment

**Document Version:** 1.0
**Date:** 2026-11-05
**Status:** Initial Assessment - Pending Resolution of Critical Issues

---

## 1. Executive Summary

This document assesses the technical feasibility of RFID-Shift-Mate 2, a next-generation RFID attendance terminal with Ethernet/PoE connectivity and integrated 2.0" TFT display.

**Overall Assessment:** The project is technically feasible with several critical dependencies that must be resolved before hardware design can proceed.

---

## 2. Critical Blockers

### 2.1 AI Management Portal Location - CRITICAL

**Status:** BLOCKER - Portal not found

**Issue:** The existing AI Management Portal has not been located in the current workspace. Without access to the portal's source code, we cannot:
- Inspect API endpoints and communication protocols
- Determine authentication mechanisms
- Identify device registration requirements
- Understand expected payload formats
- Plan the integration interface

**Impact:** Cannot proceed with Phase 2 (Portal Integration Assessment) or Phase 6 (Firmware Implementation).

**Required Action:** Provide the location of the AI Management Portal source code, or provide API documentation if the portal is hosted externally.

### 2.2 PoE Power Source - CRITICAL

**Status:** REQUIRES VERIFICATION

**Issue:** The specification mentions using a "standard TP-Link wireless router" for PoE power. However:
- Standard home/office TP-Link routers (TL-WR840N, Archer C6, Archer C7, etc.) do NOT provide PoE output
- PoE output requires either:
  - A PoE switch (enterprise-grade, expensive)
  - A PoE injector (affordable, $20-40)
  - A PoE-capable router (rare in consumer market)

**Impact:** Cannot finalize power architecture without knowing the PoE source.

**Required Action:** Clarify the PoE power source:
- Option A: Use existing TP-Link router + separate PoE injector
- Option B: Purchase a PoE switch
- Option C: Specify a PoE-capable router (if available)

**Recommendation:** Use a standard TP-Link router + PoE injector. This is the most cost-effective and flexible solution.

### 2.3 SSL Certificate Strategy - MODERATE

**Status:** REQUIRES DECISION

**Issue:** The specification mentions Cloudflare SSL certificates, but:
- Cloudflare Origin Certificates require a publicly-registered domain
- Internal-only domains (.local, .lan) cannot obtain publicly-trusted certificates
- Let's Encrypt requires public DNS validation

**Impact:** Cannot implement HTTPS without a certificate strategy.

**Options:**
1. **Public Domain with Split DNS**
   - Register a domain (e.g., rfid-shiftmate.com)
   - Use Cloudflare for external DNS
   - Use split DNS for internal resolution
   - Obtain Let's Encrypt certificate
   - Pro: Works with all browsers
   - Con: Requires domain registration ($10-15/year)

2. **Self-Signed CA**
   - Create internal CA
   - Issue certificates for internal domains
   - Distribute CA root certificate to all clients
   - Pro: No domain registration required
   - Con: Manual trust distribution, browser warnings

3. **No HTTPS (HTTP Only)**
   - Use plain HTTP for internal network
   - Accept security trade-offs
   - Pro: Simplest implementation
   - Con: No encryption, credentials in plaintext

**Recommendation:** Option 1 (Public Domain + Split DNS) if security is a priority. Option 3 (HTTP only) is acceptable for internal networks with no external access.

---

## 3. Hardware Feasibility

### 3.1 ESP32 + W5500 Integration

**Status:** FEASIBLE

**Analysis:**
- ESP32 has native W5500 support via the Ethernet library
- W5500 can be connected via SPI
- ESP32 has two SPI buses (VSPI and HSPI)
- Current device usage:
  - VSPI: MFRC522 (RFID)
  - HSPI: Available for W5500 and/or ST7789V

**Pin Assignment (Proposed):**
```
W5500 Ethernet:
- MOSI: GPIO 23 (shared with RFID)
- MISO: GPIO 19 (shared with RFID)
- SCK:  GPIO 18 (shared with RFID)
- CS:   GPIO 16 (new)
- RST:  GPIO 17 (new)

ST7789V Display:
- MOSI: GPIO 23 (shared)
- SCK:  GPIO 18 (shared)
- CS:   GPIO 4 (new, was RFID RST)
- DC:   GPIO 2 (new)
- RST:  GPIO 15 (new)
- BL:   GPIO 12 (new, backlight)

MFRC522 RFID (unchanged):
- MOSI: GPIO 23
- MISO: GPIO 19
- SCK:  GPIO 18
- CS:   GPIO 5
- RST:  GPIO 22 (was OLED SCL)
```

**SPI Bus Strategy:**
- Use VSPI for RFID (unchanged)
- Use HSPI for W5500 and ST7789V (time-multiplexed)
- Both W5500 and display have separate CS pins
- Only one device active on HSPI at a time

**Risk Level:** LOW - Standard ESP32 Ethernet integration.

### 3.2 ST7789V Display Integration

**Status:** FEASIBLE

**Analysis:**
- ST7789V is well-supported on ESP32
- Available libraries: TFT_eSPI, LovyanGFX, Adafruit ST7789
- SPI interface matches ESP32 capabilities
- 240x320 resolution is moderate (manageable memory usage)
- Display requires ~60mA at full brightness

**Library Recommendation:** TFT_eSPI
- Best performance on ESP32
- Extensive documentation
- Hardware SPI support
- DMA support for faster rendering

**Risk Level:** LOW - Mature library support.

### 3.3 PoE Power Architecture

**Status:** FEASIBLE (with correct PoE source)

**Analysis:**
- Estimated total power: 1.56W typical, 1.8W peak
- PoE 802.3af provides up to 12.95W (7x headroom)
- PoE 802.3at provides up to 25.5W (14x headroom)
- Power requirements are well within PoE capabilities

**Power Architecture (Proposed):**
```
PoE Input (48V DC)
    ↓
PoE PD Controller (Si3402 or equivalent)
    ↓
Isolated DC-DC Converter (48V → 5V)
    ↓
AMS1117-3.3 LDO (5V → 3.3V)
    ↓
ESP32, W5500, Display, RFID, etc.
```

**Required Components:**
- PoE PD controller IC (Si3402, Si3404, or module)
- Isolated DC-DC converter (48V → 5V, 3W minimum)
- 3.3V LDO regulator (AMS1117-3.3 or equivalent)
- Input/output capacitors
- Protection diodes and resistors

**Thermal Considerations:**
- PoE PD controller: ~0.5W dissipation
- DC-DC converter: ~0.2W dissipation
- LDO: ~0.3W dissipation at 200mA
- Total: ~1W - requires adequate PCB copper area for heat dissipation

**Risk Level:** MEDIUM - Requires careful PCB layout and thermal management.

### 3.4 SPI Bus Allocation

**Status:** FEASIBLE (with planning)

**Issue:** Three SPI devices (RFID, Ethernet, Display) on two SPI buses.

**Solution:**
- VSPI: MFRC2 (unchanged)
- HSPI: W5500 + ST7789V (time-multiplexed)
- Each device has dedicated CS pin
- Only one device active on HSPI at a time
- SPI bus switching handled in firmware

**Risk Level:** LOW - Standard multi-device SPI configuration.

---

## 4. Network Feasibility

### 4.1 W5500 Ethernet Integration

**Status:** FEASIBLE

**Analysis:**
- ESP32 Arduino core has native W5500 support
- Ethernet library provides standard WiFi-like API
- DHCP and static IP configuration supported
- DNS resolution supported

**Library:** Ethernet (built-in ESP32 library)
```cpp
#include <ETH.h>
#include <Ethernet.h>

// W5500 initialization
ETH.begin();
// or
Ethernet.begin(mac, ip);
```

**Risk Level:** LOW - Mature, well-documented library.

### 4.2 Local DNS Architecture

**Status:** FEASIBLE

**Analysis:**
- PC server can run BIND, dnsmasq, or Windows DNS
- Internal domain names can be configured
- Router DHCP can be configured to use PC server as DNS
- Wireless clients will receive DNS settings via DHCP

**Recommended DNS Software:** dnsmasq (Linux) or Windows DNS (if PC runs Windows)

**Configuration Steps:**
1. Assign static IP to PC server (e.g., 192.168.1.10)
2. Install and configure DNS server
3. Add A records for internal domains:
   - `rfid-device.local` → 192.168.1.20
   - `ai-portal.local` → 192.168.1.10
4. Configure router DHCP to use 192.168.1.10 as DNS server
5. Assign static IP or DHCP reservation to RFID device

**Risk Level:** LOW - Standard network configuration.

### 4.3 TP-Link Router Compatibility

**Status:** REQUIRES VERIFICATION

**Issue:** Need to confirm:
- Router model and capabilities
- Whether router supports custom DNS settings in DHCP
- Whether router has PoE ports (unlikely)

**Recommendation:**
- Assume router does NOT provide PoE
- Use separate PoE injector
- Verify router supports custom DNS in DHCP settings
- Most TP-Link routers support this via web interface

**Risk Level:** LOW - Standard router configuration.

---

## 5. Power Budget Analysis

### 5.1 Component Power Consumption

| Component | Voltage | Current (Typ) | Current (Max) | Power (Typ) | Power (Max) |
|-----------|---------|---------------|---------------|-------------|-------------|
| ESP32 (WiFi active) | 3.3V | 160mA | 240mA | 528mW | 792mW |
| W5500 Ethernet | 3.3V | 120mA | 150mA | 396mW | 495mW |
| ST7789V Display | 3.3V | 60mA | 80mA | 198mW | 264mW |
| MFRC522 RFID | 3.3V | 14mA | 20mA | 46mW | 66mW |
| DS3231 RTC | 3.3V | 0.1mA | 0.2mA | 0.3mW | 0.7mW |
| Status LED | 3.3V | 10mA | 10mA | 33mW | 33mW |
| Buzzer (active) | 3.3V | 30mA | 30mA | 99mW | 99mW |
| **Total** | | **394mA** | **530mA** | **1.30W** | **1.75W** |

### 5.2 PoE Compatibility

**PoE 802.3af (Type 1):**
- Maximum power: 12.95W
- Headroom: 7.4x (typical) / 12.95W / 1.75W
- Status: COMPATIBLE

**PoE 802.3at (Type 2):**
- Maximum power: 25.5W
- Headroom: 14.6x (typical) / 25.5W / 1.75W
- Status: COMPATIBLE

**Conclusion:** Power budget is well within PoE capabilities.

---

## 6. Firmware Feasibility

### 6.1 Existing Code Reusability

**Reusable Components from RFID-Shift-Mate 1:**
- RFID card reading logic (rfid.cpp)
- Attendance state machine (main.cpp state logic)
- Employee data structures (storage.h)
- Buzzer control logic (buzzer.cpp)
- Button handling (buttons.cpp)

**Components to Rewrite:**
- WiFi management (wifimanager.cpp) → Ethernet management
- Display driver (display.cpp) → ST7789V driver
- Web server (webserver.cpp) → May need modification for Ethernet
- Power management → Remove WiFi-specific power code

**New Components:**
- Ethernet initialization and monitoring
- ST7789V display driver
- Portal communication client
- Network status display

**Risk Level:** MEDIUM - Significant refactoring required, but core RFID logic is reusable.

### 6.2 ESP32 Resource Usage

**Flash Memory:**
- Current firmware: ~1.8MB
- Estimated new firmware: ~2.2MB (W5500 library + display library)
- ESP32 has 4MB flash
- Status: COMPATIBLE

**RAM:**
- Current usage: ~168KB free
- Estimated new usage: ~120KB free (W5500 buffers + display buffers)
- ESP32 has 520KB SRAM
- Status: COMPATIBLE

---

## 7. Risk Assessment

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| AI Portal not available | HIGH | CRITICAL | Request portal access from project owner |
| PoE source unavailable | MEDIUM | HIGH | Use PoE injector instead of router PoE |
| SPI bus conflict | LOW | MEDIUM | Use separate SPI buses or time-multiplex |
| Thermal issues | LOW | MEDIUM | Adequate PCB copper area, thermal vias |
| Display SPI speed | LOW | LOW | Use hardware SPI, DMA if needed |
| DNS resolution issues | LOW | MEDIUM | Test with dnsmasq, verify router DHCP settings |

---

## 8. Recommendations

### 8.1 Immediate Actions Required

1. **Locate AI Management Portal**
   - Provide source code location or API documentation
   - Cannot proceed without portal integration requirements

2. **Verify PoE Power Source**
   - Confirm TP-Link router model
   - Determine if PoE injector is needed
   - Specify PoE injector model if required

3. **Decide on SSL Strategy**
   - Choose between public domain + split DNS, self-signed CA, or HTTP only
   - Consider security requirements vs. implementation complexity

### 8.2 Hardware Recommendations

1. **PoE PD Controller:** Si3402-B-GM or module based on it
2. **DC-DC Converter:** 48V to 5V, 3W isolated (e.g., MORNSUN B0505XT-1W)
3. **W5500 Module:** Use breakout board for prototyping, integrate on PCB later
4. **ST7789V Display:** AITRIP 2.0" module (as specified)
5. **ESP32:** ESP32-WROOM-32 (unchanged)

### 8.3 Software Recommendations

1. **Display Library:** TFT_eSPI (best performance on ESP32)
2. **Ethernet Library:** ESP32 built-in Ethernet library
3. **JSON Library:** ArduinoJson (already used)
4. **DNS Server:** dnsmasq (if Linux) or Windows DNS (if Windows)

---

## 9. Conclusion

RFID-Shift-Mate 2 is technically feasible with the following caveats:

1. **CRITICAL:** Must locate and inspect the AI Management Portal before proceeding
2. **CRITICAL:** Must clarify PoE power source (router vs. injector)
3. **MODERATE:** Must decide on SSL certificate strategy

All other aspects (hardware, firmware, network) are feasible and well-understood.

**Recommended Next Step:** Resolve the three critical issues above before proceeding to Phase 3 (Network and Power Feasibility).

---

## Appendix A: Component Specifications

### W5500 Ethernet Controller
- Interface: SPI (up to 80MHz)
- Supported speeds: 10/100 Mbps
- Operating voltage: 3.3V
- Current: 120mA typical
- Temperature range: -40°C to +85°C

### ST7789V Display Controller
- Interface: SPI (up to 62.5MHz)
- Resolution: 240x320 pixels
- Colors: 16-bit (65K colors) or 18-bit (262K colors)
- Operating voltage: 3.3V (logic), 5V (backlight)
- Current: ~60mA at full brightness

### MFRC522 RFID Reader
- Interface: SPI (up to 10MHz)
- Frequency: 13.56MHz
- Operating voltage: 3.3V
- Current: 13-14mA typical
- Read range: ~5cm

### DS3231 RTC
- Interface: I2C (up to 400kHz)
- Operating voltage: 3.3V or 5V
- Current: 0.1mA typical
- Battery backup: CR2032 (3V)

---

**Document Prepared By:** AI Assistant
**Date:** 2026-11-05
**Version:** 1.0
