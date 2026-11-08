#include "rfid_manager.h"
#include <SPI.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

RFIDManager::RFIDManager() {
    initialized = false;
    employeeCount = 0;
}

bool RFIDManager::init(int ssPin, int rstPin) {
    Serial.println("Initializing RFID reader (MFRC522)...");
    
    SPI.begin();
    mfrc522.PCD_Init(ssPin, rstPin);
    
    // Show MFRC522 version
    byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    Serial.print("MFRC522 version: 0x");
    Serial.println(version, HEX);
    
    // Set antenna gain
    mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    
    initialized = true;
    
    // Load employee cache
    loadEmployeeCache();
    
    return true;
}

bool RFIDManager::readCard(String& uid) {
    if (!initialized) {
        return false;
    }
    
    // Check if new card is present
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }
    
    // Read card serial
    if (!mfrc522.PICC_ReadCardSerial()) {
        return false;
    }
    
    // Format UID
    uid = formatUID(mfrc522.uid.uidByte, mfrc522.uid.size);
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    
    return true;
}

String RFIDManager::getEmployeeId(String uid) {
    // Search in cache
    for (int i = 0; i < employeeCount; i++) {
        if (employeeCache[i].uid == uid) {
            return employeeCache[i].employeeId;
        }
    }
    
    // Not found
    return "";
}

String RFIDManager::getEmployeeName(String uid) {
    // Search in cache
    for (int i = 0; i < employeeCount; i++) {
        if (employeeCache[i].uid == uid) {
            return employeeCache[i].name;
        }
    }
    
    // Not found
    return "Unknown";
}

String RFIDManager::getNextAction(String employeeId) {
    // TODO: Query portal or check local state
    // For now, simple logic: if no clock-in today, clock in; else clock out
    
    // Default to clock_in
    return "clock_in";
}

String RFIDManager::getRTCTimestamp() {
    // TODO: Get from RTC
    // For now, use system time
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
    
    return String(buffer);
}

void RFIDManager::setLastUID(String uid) {
    lastUID = uid;
}

String RFIDManager::getLastUID() {
    return lastUID;
}

bool RFIDManager::loadEmployeeCache() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return false;
    }
    
    if (!SPIFFS.exists("/employees.json")) {
        Serial.println("Employee cache not found");
        return false;
    }
    
    File file = SPIFFS.open("/employees.json", "r");
    if (!file) {
        Serial.println("Failed to open employee cache");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.println("Failed to parse employee cache");
        return false;
    }
    
    JsonArray employees = doc["employees"];
    employeeCount = 0;
    
    for (JsonObject emp : employees) {
        if (employeeCount >= MAX_CACHED_EMPLOYEES) {
            break;
        }
        
        employeeCache[employeeCount].uid = emp["uid"].as<String>();
        employeeCache[employeeCount].employeeId = emp["employeeId"].as<String>();
        employeeCache[employeeCount].name = emp["name"].as<String>();
        employeeCount++;
    }
    
    Serial.print("Loaded ");
    Serial.print(employeeCount);
    Serial.println(" employees into cache");
    
    return true;
}

bool RFIDManager::saveEmployeeCache() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return false;
    }
    
    File file = SPIFFS.open("/employees.json", "w");
    if (!file) {
        Serial.println("Failed to open employee cache for writing");
        return false;
    }
    
    JsonDocument doc;
    JsonArray employees = doc["employees"].to<JsonArray>();
    
    for (int i = 0; i < employeeCount; i++) {
        JsonObject emp = employees.add<JsonObject>();
        emp["uid"] = employeeCache[i].uid;
        emp["employeeId"] = employeeCache[i].employeeId;
        emp["name"] = employeeCache[i].name;
    }
    
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write employee cache");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

String RFIDManager::formatUID(byte* buffer, byte bufferSize) {
    String result = "";
    for (byte i = 0; i < bufferSize; i++) {
        if (buffer[i] < 0x10) {
            result += "0";
        }
        result += String(buffer[i], HEX);
    }
    result.toUpperCase();
    return result;
}
