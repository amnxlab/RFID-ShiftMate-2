#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <MFRC522.h>

class RFIDManager {
public:
    RFIDManager();
    
    bool init(int ssPin, int rstPin);
    bool readCard(String& uid);
    String getEmployeeId(String uid);
    String getEmployeeName(String uid);
    String getNextAction(String employeeId);
    String getRTCTimestamp();
    
    void setLastUID(String uid);
    String getLastUID();
    
    bool loadEmployeeCache();
    bool saveEmployeeCache();
    
private:
    MFRC522 mfrc522;
    String lastUID;
    bool initialized;
    
    // Employee cache: UID -> EmployeeId, Name
    struct EmployeeCache {
        String uid;
        String employeeId;
        String name;
    };
    
    static const int MAX_CACHED_EMPLOYEES = 100;
    EmployeeCache employeeCache[MAX_CACHED_EMPLOYEES];
    int employeeCount;
    
    String formatUID(byte* buffer, byte bufferSize);
};

#endif // RFID_MANAGER_H
