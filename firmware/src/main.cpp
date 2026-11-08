/*
 * RFID-Shift-Mate 2 Firmware
 * 
 * Next-generation RFID attendance terminal with:
 * - Ethernet connectivity (W5500)
 * - PoE power input
 * - 2.0" ST7789V TFT display
 * - MFRC522 RFID reader
 * - Portal integration for attendance tracking
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ETH.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "ethernet_manager.h"
#include "portal_client.h"
#include "rfid_manager.h"
#include "display_manager.h"
#include "attendance_manager.h"
#include "button_manager.h"
#include "buzzer_manager.h"

// ============================================================================
// Global Objects
// ============================================================================

EthernetManager ethernet;
PortalClient portal;
RFIDManager rfid;
DisplayManager display;
AttendanceManager attendance;
ButtonManager buttons;
BuzzerManager buzzer;

// ============================================================================
// State Machine
// ============================================================================

enum DeviceState {
    STATE_BOOT,
    STATE_ETHERNET_INIT,
    STATE_PORTAL_REGISTER,
    STATE_IDLE,
    STATE_SCAN_WAIT,
    STATE_PROCESSING,
    STATE_SENDING,
    STATE_ERROR
};

DeviceState currentState = STATE_BOOT;
unsigned long stateTimer = 0;

// ============================================================================
// Forward Declarations
// ============================================================================

void handleStateBoot();
void handleStateEthernetInit();
void handleStatePortalRegister();
void handleStateIdle();
void handleStateScanWait();
void handleStateProcessing();
void handleStateSending();
void handleStateError();

void processRFIDScan();
void sendAttendanceToPortal();

// ============================================================================
// Setup
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n========================================");
    Serial.println("RFID-Shift-Mate 2");
    Serial.println("Firmware Version: " + String(FIRMWARE_VERSION));
    Serial.println("Build: " + String(FIRMWARE_BUILD_DATE) + " " + String(FIRMWARE_BUILD_TIME));
    Serial.println("Device ID: " + String(DEVICE_ID));
    Serial.println("========================================\n");
    
    // Initialize managers
    buzzer.init(BUZZER_PIN);
    buttons.init(BTN_UP_PIN, BTN_DOWN_PIN, BTN_SELECT_PIN);
    display.init();
    rfid.init(RFID_SS_PIN, RFID_RST_PIN);
    
    // Show boot screen
    display.showBootScreen(DEVICE_ID, FIRMWARE_VERSION);
    buzzer.beep(100);
    
    // Initialize attendance manager
    attendance.init();
    
    // Start Ethernet initialization
    currentState = STATE_ETHERNET_INIT;
    stateTimer = millis();
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    // Update managers
    buttons.update();
    buzzer.update();
    display.update();
    
    // State machine
    switch (currentState) {
        case STATE_BOOT:
            handleStateBoot();
            break;
            
        case STATE_ETHERNET_INIT:
            handleStateEthernetInit();
            break;
            
        case STATE_PORTAL_REGISTER:
            handleStatePortalRegister();
            break;
            
        case STATE_IDLE:
            handleStateIdle();
            break;
            
        case STATE_SCAN_WAIT:
            handleStateScanWait();
            break;
            
        case STATE_PROCESSING:
            handleStateProcessing();
            break;
            
        case STATE_SENDING:
            handleStateSending();
            break;
            
        case STATE_ERROR:
            handleStateError();
            break;
    }
}

// ============================================================================
// State Handlers
// ============================================================================

void handleStateBoot() {
    // Initial boot state, transition to Ethernet init
    currentState = STATE_ETHERNET_INIT;
    stateTimer = millis();
}

void handleStateEthernetInit() {
    display.showStatus("Initializing Ethernet...", "Please wait");
    
    if (ethernet.init()) {
        Serial.println("Ethernet initialized successfully");
        Serial.print("IP Address: ");
        Serial.println(ethernet.getLocalIP());
        
        display.showStatus("Ethernet Connected", ethernet.getLocalIP().toString());
        buzzer.beepSuccess();
        
        // Check if device is registered
        if (portal.isRegistered()) {
            currentState = STATE_IDLE;
        } else {
            currentState = STATE_PORTAL_REGISTER;
        }
        stateTimer = millis();
    } else {
        // Timeout after 10 seconds
        if (millis() - stateTimer > ETH_CONNECT_TIMEOUT_MS) {
            Serial.println("Ethernet initialization timeout");
            display.showError("Ethernet Failed", "Check connection");
            currentState = STATE_ERROR;
            stateTimer = millis();
        }
    }
}

void handleStatePortalRegister() {
    display.showStatus("Registering Device", "With portal...");
    
    if (portal.registerDevice(DEVICE_ID, DEVICE_NAME, DEVICE_LOCATION)) {
        Serial.println("Device registered successfully");
        Serial.print("API Key: ");
        Serial.println(portal.getApiKey());
        
        display.showStatus("Registration Complete", "Starting...");
        buzzer.beepSuccess();
        
        currentState = STATE_IDLE;
        stateTimer = millis();
    } else {
        Serial.println("Device registration failed");
        display.showError("Registration Failed", "Check portal");
        
        // Retry after 5 seconds
        if (millis() - stateTimer > 5000) {
            stateTimer = millis();
        }
    }
}

void handleStateIdle() {
    static unsigned long lastHeartbeat = 0;
    static unsigned long lastSync = 0;
    
    // Show idle screen
    display.showIdleScreen(ethernet.getLocalIP().toString(), attendance.getTodayCount());
    
    // Send heartbeat every 5 minutes
    if (millis() - lastHeartbeat > PORTAL_HEARTBEAT_INTERVAL_MS) {
        portal.sendHeartbeat(
            DEVICE_ID,
            millis() / 1000,  // uptime
            ESP.getFreeHeap(),
            attendance.getLastScanTime(),
            attendance.getTodayCount(),
            ethernet.isConnected()
        );
        lastHeartbeat = millis();
    }
    
    // Sync employee list every 5 minutes
    if (millis() - lastSync > PORTAL_SYNC_INTERVAL_MS) {
        if (portal.syncEmployees()) {
            Serial.println("Employee list synced");
        }
        lastSync = millis();
    }
    
    // Check for RFID scan button
    if (buttons.wasPressed(BTN_SELECT)) {
        currentState = STATE_SCAN_WAIT;
        stateTimer = millis();
        display.showStatus("Ready to Scan", "Tap RFID card");
        buzzer.beepReady();
    }
    
    // Check for pending attendance records to send
    if (attendance.hasPendingRecords()) {
        sendAttendanceToPortal();
    }
}

void handleStateScanWait() {
    String uid = "";
    
    // Check for RFID card
    if (rfid.readCard(uid)) {
        Serial.print("Card detected: ");
        Serial.println(uid);
        
        display.showStatus("Card Detected", uid);
        buzzer.beepAcknowledge();
        
        // Process the scan
        currentState = STATE_PROCESSING;
        stateTimer = millis();
        
        // Store UID for processing
        rfid.setLastUID(uid);
    }
    
    // Timeout after 10 seconds
    if (millis() - stateTimer > 10000) {
        Serial.println("Scan timeout");
        display.showStatus("Timeout", "Returning to idle");
        buzzer.beepError();
        
        currentState = STATE_IDLE;
        stateTimer = millis();
    }
    
    // Cancel on button press
    if (buttons.wasPressed(BTN_SELECT)) {
        currentState = STATE_IDLE;
        stateTimer = millis();
    }
}

void handleStateProcessing() {
    String uid = rfid.getLastUID();
    
    // Map UID to employee ID
    String employeeId = rfid.getEmployeeId(uid);
    
    if (employeeId == "") {
        // Unknown card
        Serial.println("Unknown card: " + uid);
        display.showError("Unknown Card", uid);
        buzzer.beepError();
        
        delay(2000);
        currentState = STATE_IDLE;
        stateTimer = millis();
        return;
    }
    
    // Get employee name
    String employeeName = rfid.getEmployeeName(uid);
    
    // Determine action (clock in or out)
    String action = attendance.getNextAction(employeeId);
    
    Serial.print("Employee: ");
    Serial.print(employeeId);
    Serial.print(" (");
    Serial.print(employeeName);
    Serial.print(") - ");
    Serial.println(action);
    
    // Create attendance record
    AttendanceRecord record;
    record.employeeId = employeeId;
    record.employeeName = employeeName;
    record.action = action;
    record.timestamp = rfid.getRTCTimestamp();
    record.rfidUid = uid;
    record.deviceId = DEVICE_ID;
    record.deviceLocation = DEVICE_LOCATION;
    
    // Add to queue
    attendance.addRecord(record);
    
    // Show success
    display.showAttendanceSuccess(employeeName, action);
    buzzer.beepSuccess();
    
    delay(2000);
    currentState = STATE_IDLE;
    stateTimer = millis();
}

void handleStateSending() {
    // Send pending attendance records to portal
    if (attendance.sendPendingToPortal(portal)) {
        Serial.println("Attendance records sent successfully");
        display.showStatus("Sent", attendance.getPendingCount() + " records");
        buzzer.beepSuccess();
    } else {
        Serial.println("Failed to send attendance records");
        display.showError("Send Failed", "Will retry");
        buzzer.beepError();
    }
    
    delay(1000);
    currentState = STATE_IDLE;
    stateTimer = millis();
}

void handleStateError() {
    // Show error state
    display.showError("Error State", "Press SELECT to retry");
    
    // Retry on button press
    if (buttons.wasPressed(BTN_SELECT)) {
        currentState = STATE_ETHERNET_INIT;
        stateTimer = millis();
    }
    
    // Auto-retry after 30 seconds
    if (millis() - stateTimer > 30000) {
        currentState = STATE_ETHERNET_INIT;
        stateTimer = millis();
    }
}

// ============================================================================
// Helper Functions
// ============================================================================

void processRFIDScan() {
    // This is handled in STATE_PROCESSING
}

void sendAttendanceToPortal() {
    currentState = STATE_SENDING;
    stateTimer = millis();
}
