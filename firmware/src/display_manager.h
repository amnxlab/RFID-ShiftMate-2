#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

class DisplayManager {
public:
    DisplayManager();
    
    void init();
    void update();
    
    void showBootScreen(String deviceId, String version);
    void showStatus(String title, String message);
    void showError(String title, String message);
    void showIdleScreen(String ipAddress, int todayCount);
    void showAttendanceSuccess(String employeeName, String action);
    
    void setBrightness(uint8_t brightness);
    void clear();
    
private:
    TFT_eSPI tft;
    uint8_t brightness;
    unsigned long lastUpdate;
    
    void drawHeader(String title);
    void drawFooter(String left, String right);
};

#endif // DISPLAY_MANAGER_H
