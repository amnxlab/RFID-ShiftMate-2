#include "attendance_manager.h"
#include <SPIFFS.h>

AttendanceManager::AttendanceManager() {
    queueSize = 0;
    todayCount = 0;
    todayRecordCount = 0;
}

void AttendanceManager::init() {
    Serial.println("Initializing attendance manager...");
    
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return;
    }
    
    // Load pending queue
    loadQueue();
    
    // Check if it's a new day and reset counters
    if (isNewDay()) {
        resetDailyCounters();
    } else {
        loadTodayRecords();
    }
    
    Serial.print("Pending records in queue: ");
    Serial.println(queueSize);
    Serial.print("Today's scans: ");
    Serial.println(todayCount);
}

void AttendanceManager::addRecord(const AttendanceRecord& record) {
    if (queueSize >= MAX_QUEUE_SIZE) {
        Serial.println("Attendance queue is full!");
        return;
    }
    
    queue[queueSize].employeeId = record.employeeId;
    queue[queueSize].employeeName = record.employeeName;
    queue[queueSize].action = record.action;
    queue[queueSize].timestamp = record.timestamp;
    queue[queueSize].rfidUid = record.rfidUid;
    queue[queueSize].deviceId = record.deviceId;
    queue[queueSize].deviceLocation = record.deviceLocation;
    
    queueSize++;
    
    // Update today's records
    bool found = false;
    for (int i = 0; i < todayRecordCount; i++) {
        if (todayRecords[i].employeeId == record.employeeId) {
            if (record.action == "clock_in") {
                todayRecords[i].clockIn = record.timestamp;
            } else {
                todayRecords[i].clockOut = record.timestamp;
            }
            found = true;
            break;
        }
    }
    
    if (!found && todayRecordCount < MAX_TODAY_RECORDS) {
        todayRecords[todayRecordCount].employeeId = record.employeeId;
        if (record.action == "clock_in") {
            todayRecords[todayRecordCount].clockIn = record.timestamp;
            todayRecords[todayRecordCount].clockOut = "";
        } else {
            todayRecords[todayRecordCount].clockIn = "";
            todayRecords[todayRecordCount].clockOut = record.timestamp;
        }
        todayRecordCount++;
    }
    
    todayCount++;
    lastScanTime = record.timestamp;
    
    saveQueue();
    saveTodayRecords();
    
    Serial.println("Attendance record added to queue");
}

bool AttendanceManager::hasPendingRecords() {
    return queueSize > 0;
}

int AttendanceManager::getPendingCount() {
    return queueSize;
}

int AttendanceManager::getTodayCount() {
    if (isNewDay()) {
        resetDailyCounters();
    }
    return todayCount;
}

String AttendanceManager::getLastScanTime() {
    return lastScanTime;
}

bool AttendanceManager::sendPendingToPortal(PortalClient& portal) {
    if (queueSize == 0) {
        return true;
    }
    
    Serial.print("Sending ");
    Serial.print(queueSize);
    Serial.println(" pending records to portal...");
    
    // Build batch request
    JsonDocument doc;
    doc["deviceId"] = DEVICE_ID;
    JsonArray recordsArray = doc["records"].to<JsonArray>();
    
    for (int i = 0; i < queueSize; i++) {
        JsonObject record = recordsArray.add<JsonObject>();
        record["employeeId"] = queue[i].employeeId;
        record["action"] = queue[i].action;
        record["timestamp"] = queue[i].timestamp;
        record["rfidUid"] = queue[i].rfidUid;
    }
    
    // Send batch
    if (portal.sendAttendanceBatch(recordsArray)) {
        Serial.println("Batch sent successfully, clearing queue");
        queueSize = 0;
        saveQueue();
        return true;
    }
    
    Serial.println("Failed to send batch");
    return false;
}

String AttendanceManager::getNextAction(String employeeId) {
    // Check if employee has clocked in today
    for (int i = 0; i < todayRecordCount; i++) {
        if (todayRecords[i].employeeId == employeeId) {
            if (todayRecords[i].clockIn != "" && todayRecords[i].clockOut == "") {
                return "clock_out";
            } else {
                return "clock_in";
            }
        }
    }
    
    // Default to clock_in
    return "clock_in";
}

bool AttendanceManager::loadQueue() {
    if (!SPIFFS.exists(ATTENDANCE_QUEUE_PATH)) {
        queueSize = 0;
        return true;
    }
    
    File file = SPIFFS.open(ATTENDANCE_QUEUE_PATH, "r");
    if (!file) {
        Serial.println("Failed to open attendance queue");
        return false;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.println("Failed to parse attendance queue");
        return false;
    }
    
    JsonArray records = doc["records"];
    queueSize = 0;
    
    for (JsonObject record : records) {
        if (queueSize >= MAX_QUEUE_SIZE) {
            break;
        }
        
        queue[queueSize].employeeId = record["employeeId"] | "";
        queue[queueSize].employeeName = record["employeeName"] | "";
        queue[queueSize].action = record["action"] | "";
        queue[queueSize].timestamp = record["timestamp"] | "";
        queue[queueSize].rfidUid = record["rfidUid"] | "";
        queue[queueSize].deviceId = record["deviceId"] | "";
        queue[queueSize].deviceLocation = record["deviceLocation"] | "";
        queueSize++;
    }
    
    return true;
}

bool AttendanceManager::saveQueue() {
    File file = SPIFFS.open(ATTENDANCE_QUEUE_PATH, "w");
    if (!file) {
        Serial.println("Failed to open attendance queue for writing");
        return false;
    }
    
    JsonDocument doc;
    JsonArray records = doc["records"].to<JsonArray>();
    
    for (int i = 0; i < queueSize; i++) {
        JsonObject record = records.add<JsonObject>();
        record["employeeId"] = queue[i].employeeId;
        record["employeeName"] = queue[i].employeeName;
        record["action"] = queue[i].action;
        record["timestamp"] = queue[i].timestamp;
        record["rfidUid"] = queue[i].rfidUid;
        record["deviceId"] = queue[i].deviceId;
        record["deviceLocation"] = queue[i].deviceLocation;
    }
    
    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write attendance queue");
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

void AttendanceManager::loadTodayRecords() {
    if (!SPIFFS.exists("/today_records.json")) {
        todayRecordCount = 0;
        return;
    }
    
    File file = SPIFFS.open("/today_records.json", "r");
    if (!file) {
        todayRecordCount = 0;
        return;
    }
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        todayRecordCount = 0;
        return;
    }
    
    JsonArray records = doc["records"];
    todayRecordCount = 0;
    
    for (JsonObject record : records) {
        if (todayRecordCount >= MAX_TODAY_RECORDS) {
            break;
        }
        
        todayRecords[todayRecordCount].employeeId = record["employeeId"] | "";
        todayRecords[todayRecordCount].clockIn = record["clockIn"] | "";
        todayRecords[todayRecordCount].clockOut = record["clockOut"] | "";
        todayRecordCount++;
    }
}

void AttendanceManager::saveTodayRecords() {
    File file = SPIFFS.open("/today_records.json", "w");
    if (!file) {
        return;
    }
    
    JsonDocument doc;
    JsonArray records = doc["records"].to<JsonArray>();
    
    for (int i = 0; i < todayRecordCount; i++) {
        JsonObject record = records.add<JsonObject>();
        record["employeeId"] = todayRecords[i].employeeId;
        record["clockIn"] = todayRecords[i].clockIn;
        record["clockOut"] = todayRecords[i].clockOut;
    }
    
    serializeJson(doc, file);
    file.close();
}

bool AttendanceManager::isNewDay() {
    // TODO: Compare with stored date
    // For now, always return false
    return false;
}

void AttendanceManager::resetDailyCounters() {
    todayCount = 0;
    todayRecordCount = 0;
    saveTodayRecords();
    Serial.println("Daily counters reset");
}
