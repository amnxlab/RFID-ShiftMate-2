#include "portal_client.h"
#include <SPIFFS.h>

PortalClient::PortalClient() {
    portalUrl = PORTAL_URL;
    apiKey = PORTAL_API_KEY;
    registered = (apiKey.length() > 0);
}

bool PortalClient::registerDevice(String deviceId, String deviceName, String location) {
    Serial.println("Registering device with portal...");
    
    JsonDocument doc;
    doc["deviceId"] = deviceId;
    doc["deviceName"] = deviceName;
    doc["location"] = location;
    doc["firmwareVersion"] = FIRMWARE_VERSION;
    
    // TODO: Get MAC address from Ethernet
    doc["macAddress"] = "AA:BB:CC:DD:EE:FF";
    
    // TODO: Get IP from Ethernet
    doc["ipAddress"] = "192.168.1.100";
    
    String body;
    serializeJson(doc, body);
    
    HTTPResponse response = sendRequest("POST", "/api/devices/register", body);
    
    if (response.success && response.httpCode == 200) {
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response.body);
        
        if (!error) {
            if (respDoc["success"] == true) {
                apiKey = respDoc["device"]["apiKey"].as<String>();
                registered = true;
                saveConfig();
                return true;
            }
        }
    }
    
    return false;
}

bool PortalClient::sendAttendance(const AttendanceRecord& record) {
    Serial.println("Sending attendance to portal...");
    
    JsonDocument doc;
    doc["employeeId"] = record.employeeId;
    doc["employeeName"] = record.employeeName;
    doc["action"] = record.action;
    doc["timestamp"] = record.timestamp;
    doc["rfidUid"] = record.rfidUid;
    doc["deviceId"] = record.deviceId;
    doc["deviceLocation"] = record.deviceLocation;
    
    String body;
    serializeJson(doc, body);
    
    HTTPResponse response = sendRequest("POST", "/api/attendance", body);
    
    if (response.success && response.httpCode == 200) {
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response.body);
        
        if (!error) {
            if (respDoc["success"] == true) {
                Serial.println("Attendance sent successfully");
                return true;
            }
        }
    }
    
    Serial.println("Failed to send attendance");
    return false;
}

bool PortalClient::sendAttendanceBatch(JsonArray records) {
    Serial.println("Sending attendance batch to portal...");
    
    JsonDocument doc;
    doc["deviceId"] = DEVICE_ID;
    JsonArray recordsArray = doc["records"].to<JsonArray>();
    
    for (JsonObject record : records) {
        JsonObject newRecord = recordsArray.add<JsonObject>();
        newRecord["employeeId"] = record["employeeId"];
        newRecord["action"] = record["action"];
        newRecord["timestamp"] = record["timestamp"];
        newRecord["rfidUid"] = record["rfidUid"];
    }
    
    String body;
    serializeJson(doc, body);
    
    HTTPResponse response = sendRequest("POST", "/api/attendance/batch", body);
    
    if (response.success && response.httpCode == 200) {
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response.body);
        
        if (!error) {
            if (respDoc["success"] == true) {
                Serial.println("Attendance batch sent successfully");
                return true;
            }
        }
    }
    
    Serial.println("Failed to send attendance batch");
    return false;
}

bool PortalClient::syncEmployees() {
    Serial.println("Syncing employee list from portal...");
    
    HTTPResponse response = sendRequest("GET", "/api/employees/sync");
    
    if (response.success && response.httpCode == 200) {
        JsonDocument respDoc;
        DeserializationError error = deserializeJson(respDoc, response.body);
        
        if (!error) {
            JsonArray employees = respDoc["employees"];
            Serial.print("Synced ");
            Serial.print(employees.size());
            Serial.println(" employees");
            
            // TODO: Save to local storage
            return true;
        }
    }
    
    Serial.println("Failed to sync employees");
    return false;
}

bool PortalClient::sendHeartbeat(String deviceId, unsigned long uptime, uint32_t freeHeap,
                                String lastScan, int todayCount, bool networkStatus) {
    JsonDocument doc;
    doc["deviceId"] = deviceId;
    doc["status"] = "online";
    doc["uptime"] = uptime;
    doc["freeHeap"] = freeHeap;
    doc["lastCardScan"] = lastScan;
    doc["totalScansToday"] = todayCount;
    
    JsonObject network = doc["networkStatus"].to<JsonObject>();
    network["connected"] = networkStatus;
    // TODO: Add IP and RSSI
    
    String body;
    serializeJson(doc, body);
    
    HTTPResponse response = sendRequest("POST", "/api/devices/heartbeat", body);
    
    return (response.success && response.httpCode == 200);
}

bool PortalClient::isRegistered() {
    return registered;
}

String PortalClient::getApiKey() {
    return apiKey;
}

void PortalClient::setApiKey(String key) {
    apiKey = key;
    registered = (key.length() > 0);
}

String PortalClient::getPortalUrl() {
    return portalUrl;
}

void PortalClient::setPortalUrl(String url) {
    portalUrl = url;
}

bool PortalClient::loadConfig() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return false;
    }
    
    if (!SPIFFS.exists("/portal_config.json")) {
        Serial.println("Portal config not found");
        return false;
    }
    
    File file = SPIFFS.open("/portal_config.json", "r");
    if (!file) {
        Serial.println("Failed to open portal config");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.println("Failed to parse portal config");
        return false;
    }
    
    portalUrl = doc["portalUrl"] | PORTAL_URL;
    apiKey = doc["apiKey"] | "";
    registered = (apiKey.length() > 0);
    
    return true;
}

bool PortalClient::saveConfig() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return false;
    }
    
    File file = SPIFFS.open("/portal_config.json", "w");
    if (!file) {
        Serial.println("Failed to open portal config for writing");
        return false;
    }
    
    JsonDocument doc;
    doc["portalUrl"] = portalUrl;
    doc["apiKey"] = apiKey;
    
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write portal config");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

HTTPResponse PortalClient::sendRequest(String method, String endpoint, String body) {
    HTTPResponse response;
    response.success = false;
    response.httpCode = 0;
    
    if (!registered && endpoint != "/api/devices/register") {
        Serial.println("Device not registered");
        return response;
    }
    
    WiFiClientSecure client;
    client.setInsecure();  // Skip certificate validation for internal use
    
    HTTPClient http;
    String url = portalUrl + endpoint;
    
    http.begin(client, url);
    http.setTimeout(PORTAL_REQUEST_TIMEOUT_MS);
    
    // Add headers
    http.addHeader("Content-Type", "application/json");
    if (registered) {
        http.addHeader("X-API-Key", apiKey);
    }
    http.addHeader("X-Device-ID", DEVICE_ID);
    
    int httpCode;
    if (method == "POST") {
        httpCode = http.POST(body);
    } else if (method == "GET") {
        httpCode = http.GET();
    } else {
        http.end();
        return response;
    }
    
    response.httpCode = httpCode;
    
    if (httpCode > 0) {
        response.body = http.getString();
        response.success = (httpCode >= 200 && httpCode < 300);
    } else {
        Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
    return response;
}
