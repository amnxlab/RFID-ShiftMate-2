#include "display_manager.h"
#include "config.h"

DisplayManager::DisplayManager() {
    brightness = DISPLAY_BRIGHTNESS_DEFAULT;
    lastUpdate = 0;
}

void DisplayManager::init() {
    Serial.println("Initializing display (ST7789V)...");
    
    tft.init();
    tft.setRotation(0);  // Portrait mode (240x320)
    tft.setBrightness(brightness);
    tft.fillScreen(TFT_BLACK);
    
    Serial.println("Display initialized");
}

void DisplayManager::update() {
    // Nothing to update for now
}

void DisplayManager::showBootScreen(String deviceId, String version) {
    tft.fillScreen(TFT_BLACK);
    
    // Title
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(3);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("RFID", 120, 40);
    tft.drawString("ShiftMate 2", 120, 75);
    
    // Divider
    tft.drawFastHLine(10, 110, 220, TFT_CYAN);
    
    // Device ID
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString(deviceId, 120, 140);
    
    // Version
    tft.setTextColor(TFT_DARKGREY);
    tft.setTextSize(1);
    tft.drawString("v" + version, 120, 170);
    
    // Status
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("Starting...", 120, 220);
}

void DisplayManager::showStatus(String title, String message) {
    tft.fillScreen(TFT_BLACK);
    
    drawHeader(title);
    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(message, 120, 160);
}

void DisplayManager::showError(String title, String message) {
    tft.fillScreen(TFT_BLACK);
    
    // Header with red background
    tft.fillRect(0, 0, 240, 50, TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(title, 120, 25);
    
    // Error icon
    tft.setTextColor(TFT_RED);
    tft.setTextSize(4);
    tft.drawString("X", 120, 120);
    
    // Message
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString(message, 120, 200);
}

void DisplayManager::showIdleScreen(String ipAddress, int todayCount) {
    tft.fillScreen(TFT_BLACK);
    
    drawHeader("RFID ShiftMate 2");
    
    // IP Address
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("IP: " + ipAddress, 120, 100);
    
    // Today's count
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(3);
    tft.drawString(String(todayCount), 120, 160);
    tft.setTextSize(1);
    tft.drawString("scans today", 120, 185);
    
    // Instructions
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.drawString("Press SELECT", 120, 240);
    tft.drawString("to scan", 120, 265);
}

void DisplayManager::showAttendanceSuccess(String employeeName, String action) {
    tft.fillScreen(TFT_BLACK);
    
    // Success header
    tft.fillRect(0, 0, 240, 50, TFT_GREEN);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Success!", 120, 25);
    
    // Checkmark icon
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(4);
    tft.drawString("✓", 120, 110);
    
    // Employee name
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString(employeeName, 120, 170);
    
    // Action
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(2);
    String actionText = (action == "clock_in") ? "Clocked IN" : "Clocked OUT";
    tft.drawString(actionText, 120, 210);
}

void DisplayManager::setBrightness(uint8_t brightness) {
    this->brightness = brightness;
    tft.setBrightness(brightness);
}

void DisplayManager::clear() {
    tft.fillScreen(TFT_BLACK);
}

void DisplayManager::drawHeader(String title) {
    tft.fillRect(0, 0, 240, 40, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(title, 120, 20);
}

void DisplayManager::drawFooter(String left, String right) {
    tft.fillRect(0, 280, 240, 40, TFT_DARKGREY);
    tft.setTextColor(TFT_LIGHTGREY);
    tft.setTextSize(1);
    
    tft.setTextDatum(BL_DATUM);
    tft.drawString(left, 10, 315);
    
    tft.setTextDatum(BR_DATUM);
    tft.drawString(right, 230, 315);
}
