#ifndef ATTENDANCE_MANAGER_H
#define ATTENDANCE_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "portal_client.h"

class AttendanceManager {
public:
    AttendanceManager();
    
    void init();
    
    void addRecord(const AttendanceRecord& record);
    bool hasPendingRecords();
    int getPendingCount();
    int getTodayCount();
    String getLastScanTime();
    
    bool sendPendingToPortal(PortalClient& portal);
    
    bool loadQueue();
    bool saveQueue();
    
    String getNextAction(String employeeId);
    
private:
    struct QueueRecord {
        String employeeId;
        String employeeName;
        String action;
        String timestamp;
        String rfidUid;
        String deviceId;
        String deviceLocation;
    };
    
    static const int MAX_QUEUE_SIZE = 1000;
    QueueRecord queue[MAX_QUEUE_SIZE];
    int queueSize;
    
    int todayCount;
    String lastScanTime;
    
    // Track today's attendance for clock-in/out logic
    struct TodayRecord {
        String employeeId;
        String clockIn;
        String clockOut;
    };
    
    static const int MAX_TODAY_RECORDS = 200;
    TodayRecord todayRecords[MAX_TODAY_RECORDS];
    int todayRecordCount;
    
    void loadTodayRecords();
    void saveTodayRecords();
    bool isNewDay();
    void resetDailyCounters();
};

#endif // ATTENDANCE_MANAGER_H
