# RFID-Shift-Mate 2 - Project Plan

**Document Version:** 1.0
**Date:** 2026-11-05
**Status:** Planning Phase

---

## Project Timeline Estimate

| Phase | Duration | Dependencies | Status |
|-------|----------|--------------|--------|
| Phase 1: Assessment | 1-2 days | Portal source code | Blocked |
| Phase 2: Network/Power Feasibility | 2-3 days | PoE source clarification | Pending |
| Phase 3: Hardware Design | 1-2 weeks | Phase 2 complete | Not started |
| Phase 4: Firmware Implementation | 2-3 weeks | Phase 3 complete | Not started |
| Phase 5: Integration & Testing | 1-2 weeks | Phase 4 complete | Not started |
| **Total Estimated** | **5-8 weeks** | | |

---

## Phase 1: Existing Project Assessment (CURRENT)

### Objectives
- [x] Create RFID-Shift-Mate 2 repository structure
- [x] Initialize Git repository
- [x] Document project requirements
- [x] Create feasibility assessment
- [ ] Inspect existing RFID-Shift-Mate firmware
- [ ] **Locate AI Management Portal** (CRITICAL BLOCKER)
- [ ] Inspect portal integration requirements
- [ ] Document reusable components
- [ ] Define integration interface specification

### Deliverables
- [ ] Assessment document (existing firmware)
- [ ] Portal integration specification
- [ ] List of reusable components
- [ ] Component list for RFID-Shift-Mate 2

### Critical Blockers
1. **AI Management Portal location unknown** - Cannot inspect integration requirements
2. **PoE power source unclear** - Cannot finalize power architecture

---

## Phase 2: Network and Power Feasibility

### Objectives
- [ ] Verify TP-Link router model and capabilities
- [ ] Determine PoE power source (injector vs. switch)
- [ ] Validate W5500 module compatibility
- [ ] Test ST7789V display with ESP32
- [ ] Design DNS architecture
- [ ] Plan IP address scheme
- [ ] Decide on SSL certificate strategy
- [ ] Validate SPI bus allocation

### Deliverables
- [ ] Network architecture document
- [ ] Power budget calculation
- [ ] Component validation report
- [ ] DNS configuration guide
- [ ] IP address plan
- [ ] SSL certificate recommendation

### Testing Requirements
- [ ] W5500 Ethernet connectivity test
- [ ] ST7789V display test
- [ ] PoE power delivery test (if hardware available)
- [ ] DNS resolution test
- [ ] Portal connectivity test (if portal available)

---

## Phase 3: Hardware Design

### Objectives
- [ ] Design schematic
- [ ] Create PCB layout
- [ ] Select PoE PD controller
- [ ] Select DC-DC converter
- [ ] Design power distribution network
- [ ] Validate thermal management
- [ ] Create Bill of Materials (BOM)
- [ ] Order prototype PCB

### Deliverables
- [ ] Schematic PDF and source files
- [ ] PCB layout files (KiCad or similar)
- [ ] BOM spreadsheet
- [ ] Assembly instructions
- [ ] Power architecture diagram
- [ ] Thermal analysis report

### Design Constraints
- 4-layer PCB recommended (for PoE isolation)
- Minimum trace width: 0.2mm (signal), 0.5mm (power)
- PoE input isolation: 1.5kV minimum
- Operating temperature: 0°C to 50°C

---

## Phase 4: Firmware Implementation

### Objectives
- [ ] Set up PlatformIO project
- [ ] Implement W5500 Ethernet driver
- [ ] Implement ST7789V display driver
- [ ] Port RFID functionality from original project
- [ ] Port attendance state machine
- [ ] Implement portal communication client
- [ ] Implement network monitoring
- [ ] Implement DHCP/static IP configuration
- [ ] Implement network reconnection logic
- [ ] Implement device status reporting
- [ ] Port button handling and buzzer control

### Deliverables
- [ ] Complete firmware source code
- [ ] PlatformIO configuration
- [ ] Build instructions
- [ ] Firmware documentation
- [ ] API documentation (device endpoints)

### Code Structure
```
firmware/
├── src/
│   ├── main/
│   │   └── main.cpp              # Entry point, state machine
│   ├── rfid/
│   │   ├── rfid.cpp              # MFRC522 driver
│   │   └── rfid.h
│   ├── ethernet/
│   │   ├── ethernet_manager.cpp  # W5500 initialization
│   │   └── ethernet_manager.h
│   ├── network/
│   │   ├── network_manager.cpp   # Network monitoring
│   │   └── network_manager.h
│   ├── portal_integration/
│   │   ├── portal_client.cpp     # Portal communication
│   │   └── portal_client.h
│   ├── display/
│   │   ├── display.cpp           # ST7789V driver
│   │   └── display.h
│   └── system/
│       ├── config.cpp            # Configuration management
│       ├── storage.cpp           # LittleFS operations
│       └── buzzer.cpp            # Buzzer control
├── include/
│   └── config.h                  # Pin definitions, constants
├── lib/                          # Custom libraries
└── platformio.ini                # Build configuration
```

---

## Phase 5: Integration and Testing

### Objectives
- [ ] Flash firmware to prototype hardware
- [ ] Test Ethernet connectivity
- [ ] Test DHCP/static IP assignment
- [ ] Test RFID card reading
- [ ] Test display functionality
- [ ] Configure DNS server
- [ ] Test domain name resolution
- [ ] Test portal-to-device communication
- [ ] Test device-to-portal communication
- [ ] Test network disconnection recovery
- [ ] Test PoE power stability
- [ ] End-to-end integration test

### Deliverables
- [ ] Test plan document
- [ ] Test results report
- [ ] Bug list and fixes
- [ ] Integration validation report

### Test Scenarios
1. **Power-on Test**: Device boots, Ethernet connects, display shows status
2. **RFID Test**: Card scan detected, attendance recorded, display updated
3. **Network Test**: DHCP assignment, DNS resolution, portal connectivity
4. **Recovery Test**: Ethernet disconnect/reconnect, device recovers
5. **PoE Test**: Power delivery stable under load, thermal check
6. **Integration Test**: Portal can identify device, read data, send commands

---

## Phase 6: Documentation and Release

### Objectives
- [ ] Write user manual
- [ ] Write installation guide
- [ ] Write network configuration guide
- [ ] Write firmware development guide
- [ ] Create hardware assembly guide
- [ ] Create troubleshooting guide
- [ ] Prepare GitHub release
- [ ] Create demo video (optional)

### Deliverables
- [ ] User manual (PDF)
- [ ] Installation guide (PDF)
- [ ] Network configuration guide (PDF)
- [ ] Firmware development guide (Markdown)
- [ ] Hardware assembly guide (PDF)
- [ ] Troubleshooting guide (PDF)
- [ ] GitHub release with all files

---

## Risk Mitigation Plan

### Risk 1: AI Portal Not Available
**Probability:** HIGH
**Impact:** CRITICAL
**Mitigation:**
- Request portal source code from project owner
- If unavailable, request API documentation
- If neither available, define a generic REST API interface
- Design firmware to be configurable for different portal APIs

### Risk 2: PoE Source Unavailable
**Probability:** MEDIUM
**Impact:** HIGH
**Mitigation:**
- Use PoE injector instead of router PoE
- Design PCB to accept both PoE and USB power (fallback)
- Provide clear PoE source requirements in documentation

### Risk 3: SPI Bus Conflicts
**Probability:** LOW
**Impact:** MEDIUM
**Mitigation:**
- Use separate SPI buses (VSPI for RFID, HSPI for W5500 + display)
- Time-multiplex W5500 and display on HSPI
- Test SPI performance early in development
- Have backup plan: SPI multiplexer IC if needed

### Risk 4: Thermal Issues
**Probability:** LOW
**Impact:** MEDIUM
**Mitigation:**
- Use adequate PCB copper area for heat dissipation
- Add thermal vias under power components
- Test thermal performance with thermal camera
- Add heatsinks if necessary

### Risk 5: Display Performance
**Probability:** LOW
**Impact:** LOW
**Mitigation:**
- Use hardware SPI (not software SPI)
- Enable DMA if supported
- Optimize display refresh rate
- Use partial screen updates where possible

---

## Resource Requirements

### Hardware (for development)
- 1x ESP32 development board
- 1x W5500 Ethernet module
- 1x ST7789V 2.0" TFT display
- 1x MFRC522 RFID reader
- 1x PoE PD controller module (or PoE injector for testing)
- 1x DS3231 RTC module
- 1x Passive buzzer
- 1x Status LED
- 3x Push buttons
- Jumper wires, breadboard, power supply

### Software
- PlatformIO (already installed)
- KiCad (for PCB design) - free
- Git (already installed)
- Text editor / IDE

### Infrastructure
- TP-Link router (existing)
- PC server for portal and DNS (existing)
- PoE injector or PoE switch (if needed)
- Network switch with Ethernet ports

---

## Success Criteria

The project is considered successful when:
1. Hardware prototype is assembled and powered via PoE
2. Device connects to Ethernet and obtains IP address
3. Device is accessible via internal domain name
4. RFID cards can be read and attendance recorded
5. Display shows correct information
6. AI Management Portal can communicate with device
7. Wireless clients can access device via domain name
8. Device recovers from network disconnection
9. PoE power delivery is stable under all operating conditions
10. System operates reliably for 24+ hours

---

## Approval Required

Before proceeding, the following decisions must be made:

1. **AI Management Portal:** Provide source code location or API documentation
2. **PoE Power Source:** Confirm router model and whether PoE injector is acceptable
3. **SSL Strategy:** Choose between public domain + split DNS, self-signed CA, or HTTP only
4. **Internal Domain Names:** Select domain names for RFID device and portal

---

**Document Prepared By:** AI Assistant
**Date:** 2026-11-05
**Version:** 1.0
