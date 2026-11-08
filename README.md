# RFID-Shift-Mate 2

Next-generation RFID attendance terminal with Ethernet/PoE connectivity, integrated 2.0" TFT display, and centralized AI Management Portal integration.

## Project Status

**Phase 1: Project Assessment and Planning** (In Progress)

This repository is currently in the initial planning phase. No hardware designs or firmware implementation have been completed yet.

### Current Status
- [x] Repository structure created
- [x] Git repository initialized
- [ ] Existing RFID-Shift-Mate assessment
- [ ] AI Management Portal integration assessment
- [ ] Network architecture feasibility study
- [ ] PoE power feasibility verification
- [ ] Hardware component validation
- [ ] PCB design
- [ ] Firmware implementation
- [ ] Integration testing
- [ ] Documentation

## Project Overview

RFID-Shift-Mate 2 is a complete redesign of the original RFID-Shift-Mate project, transitioning from WiFi to Ethernet connectivity with PoE power delivery.

### Key Changes from Original
- **Network**: WiFi AP mode → Ethernet with W5500 controller
- **Power**: USB/DC power → PoE (Power over Ethernet)
- **Display**: 0.96" OLED → 2.0" ST7789V IPS TFT (240x320)
- **Architecture**: Standalone → Centralized with PC-hosted AI portal
- **DNS**: None → Local DNS with internal domain names
- **SSL**: None → Cloudflare-compatible HTTPS

### System Architecture
```
                  INTERNET (OPTIONAL)
                          |
                   TP-LINK ROUTER
                   /      |       \
                  /       |        \
             Ethernet   Ethernet   Wi-Fi
                |          |         |
          RFID-Shift    PC SERVER   Wireless
             Mate 2        |        Clients
                |          |
             W5500      DNS SERVER
                |          |
             ESP32     EXISTING AI
                |      MANAGEMENT
             RFID      PORTAL
```

## Critical Constraints

1. **Do not modify the original RFID-Shift-Mate repository**
2. **Do not rebuild the AI Management Portal** - it already exists
3. **Verify PoE compatibility** before hardware design
4. **Inspect existing portal integration** before implementation
5. **Validate all assumptions** before committing to architecture

## Development Phases

### Phase 1: Assessment (Current)
- Inspect existing RFID-Shift-Mate firmware
- Locate and inspect AI Management Portal
- Assess integration requirements
- Verify PoE feasibility
- Validate component compatibility

### Phase 2: Network and Power Feasibility
- TP-Link router PoE capabilities
- Power budget calculation
- W5500 integration requirements
- DNS/DHCP architecture

### Phase 3: Hardware Design
- Schematic design
- PCB layout
- PoE power circuit design
- Component selection and validation

### Phase 4: Firmware Implementation
- W5500 Ethernet driver
- ST7789V display driver
- RFID functionality preservation
- Portal integration interface
- Network management

### Phase 5: Integration and Testing
- Network configuration
- DNS setup
- Portal connectivity testing
- End-to-end validation

## Hardware Specifications (Planned)

### Microcontroller
- ESP32-WROOM-32

### Ethernet
- W5500 Ethernet controller
- SPI interface
- RJ45 connector with magnetics

### Power
- PoE input (IEEE 802.3af/at compatible)
- PoE PD controller (Si3402 or equivalent)
- Isolated DC-DC converter
- 5V and 3.3V rails

### Display
- 2.0" ST7789V IPS TFT
- 240x320 resolution
- SPI interface
- RGB color

### RFID
- MFRC522 reader
- 13.56MHz
- SPI interface
- Card detection and UID reading

### Other Peripherals
- DS3231 RTC with battery backup
- Passive buzzer for audio feedback
- Status LED
- 3x navigation buttons (UP, DOWN, SELECT)

## Power Budget Estimate

| Component | Voltage | Current | Power |
|-----------|---------|---------|-------|
| ESP32 (WiFi active) | 3.3V | 240mA | 792mW |
| W5500 Ethernet | 3.3V | 120mA | 396mW |
| ST7789V Display | 3.3V | 60mA | 198mW |
| MFRC522 RFID | 3.3V | 14mA | 46mW |
| DS3231 RTC | 3.3V | 0.1mA | 0.3mW |
| Status LED | 3.3V | 10mA | 33mW |
| Buzzer (active) | 3.3V | 30mA | 99mW |
| **Total (typical)** | | | **1.56W** |
| **Total (peak)** | | | **1.8W** |

*Note: PoE 802.3af provides up to 12.95W, providing 7x headroom.*

## Network Architecture (Planned)

### IP Assignment
- RFID-Shift-Mate 2: Static IP or DHCP reservation
- PC Server: Static IP
- Router: DHCP server (192.168.1.1)

### DNS Configuration
- PC server runs local DNS (BIND or dnsmasq)
- Internal domain: `rfid-device.local` (example)
- Portal domain: `ai-portal.local` (example)
- Router DHCP configured to use PC server DNS

### Communication
- RFID device exposes HTTP API on port 80
- Portal connects to RFID device via HTTP/HTTPS
- Wireless clients access both via domain names

## Repository Structure

```
RFID-Shift-Mate-2/
├── firmware/              # ESP32 firmware
│   ├── src/
│   ├── include/
│   ├── lib/
│   └── platformio.ini
├── hardware/              # PCB and schematic files
│   ├── schematics/
│   ├── pcb/
│   ├── components/
│   └── bom/
├── network/               # Network configuration
│   ├── architecture/
│   ├── dns/
│   ├── ip_configuration/
│   └── ssl/
├── integration/           # Portal integration docs
│   ├── ai_portal_interface/
│   ├── api_documentation/
│   └── communication_protocol/
├── docs/                  # Project documentation
│   ├── architecture/
│   ├── hardware/
│   ├── firmware/
│   ├── networking/
│   ├── installation/
│   └── testing/
├── tests/                 # Test procedures and results
├── README.md
└── CHANGELOG.md
```

## Dependencies

### Hardware
- ESP32 development board (for prototyping)
- W5500 Ethernet module
- ST7789V 2.0" TFT display
- MFRC522 RFID reader
- PoE PD controller module
- DS3231 RTC module

### Software
- PlatformIO
- ESP32 Arduino Core
- TFT_eSPI or LovyanGFX library
- Ethernet library for ESP32
- ArduinoJson

### Infrastructure
- TP-Link router (PoE-capable or with PoE injector)
- PC server for AI portal and DNS
- Existing AI Management Portal

## Resolved Decisions

### 1. AI Management Portal Integration
**Status**: RESOLVED - Interface Defined
- Portal integration API specification created (see `integration/ai_portal_interface/api-specification.md`)
- Device pushes attendance data to portal via HTTP/HTTPS
- Portal will be implemented separately by the user
- Device needs to:
  - Read RFID tag UID
  - Map UID to Employee ID (from local cache or portal query)
  - POST attendance record to portal endpoint
  - Include: employeeId, action (clock_in/clock_out), timestamp, rfidUid, deviceId
- Portal exposes endpoints for device registration, attendance push, employee sync, heartbeat
- Device implements client to call portal APIs

### 2. PoE Power Source
**Status**: RESOLVED - PoE+ Switch
- Will use 10-port PoE+ switch (802.3af/at compliant, 120W total budget)
- Switch provides PoE power to RFID devices via Ethernet
- PCB design includes:
  - PoE PD controller (Si3402 or equivalent)
  - Isolated DC-DC converter (48V → 5V)
  - 3.3V LDO for ESP32 and peripherals
- Power budget: ~1.8W peak per device (well within 120W switch capacity)

### 3. SSL Certificate Strategy
**Status**: RESOLVED - Public Domain with Internal DNS
- Register a public domain (e.g., shiftmate.local or real TLD)
- Use Cloudflare Origin Certificate for HTTPS
- Internal DNS server resolves domain to local IPs
- Certificate is valid because domain is publicly registered
- DNS resolution is local-only (split DNS architecture)

### 4. SPI Bus Allocation
**Status**: Requires Planning
- ESP32 has 2 SPI buses (VSPI and HSPI)
- Current devices on VSPI: MFRC522 (RFID)
- Need to add: W5500 (Ethernet), ST7789V (Display)
- Options:
  - Use HSPI for W5500 and/or display
  - Use SPI multiplexer
  - Time-multiplex devices on single bus (not recommended)

## Next Steps

1. **Locate AI Management Portal**
   - Search for portal source code on this system
   - If not found, request access from project owner
   - Inspect API documentation and integration requirements

2. **Verify PoE Equipment**
   - Identify specific TP-Link router model
   - Check if router has PoE ports
   - If not, specify PoE injector requirements

3. **Component Validation**
   - Verify W5500 module compatibility with ESP32
   - Confirm ST7789V display SPI requirements
   - Test PoE PD controller module

4. **Network Architecture Validation**
   - Test local DNS setup with dnsmasq
   - Verify domain resolution for internal names
   - Test portal-to-device communication

## License

This project will be licensed under the MIT License (to be confirmed).

## Contact

Project Lead: amnxlab
Email: ahmedmohamed6071@gmail.com
