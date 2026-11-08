#ifndef PORTAL_CLIENT_H
#define PORTAL_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "config.h"

struct AttendanceRecord {
    String employeeId;
    String employeeName;
    String action;  // "clock_in" or "clock_out"
    String timestamp;
    String rfidUid;
    String deviceId;
    String deviceLocation;
};

class PortalClient {
public:
    PortalClient();
    
    bool registerDevice(String deviceId, String deviceName, String location);
    bool sendAttendance(const AttendanceRecord& record);
    bool sendAttendanceBatch(JsonArray records);
    bool syncEmployees();
    bool sendHeartbeat(String deviceId, unsigned long uptime, uint32_t freeHeap, 
                      String lastScan, int todayCount, bool networkStatus);
    
    bool isRegistered();
    String getApiKey();
    void setApiKey(String key);
    String getPortalUrl();
    void setPortalUrl(String url);
    
    bool loadConfig();
    bool saveConfig();
    
private:
    String portalUrl;
    String apiKey;
    bool registered;
    
    HTTPResponse sendRequest(String method, String endpoint, String body = "");
};

#endif // PORTAL_CLIENT_H
