#include "buzzer_manager.h"

BuzzerManager::BuzzerManager() {
    pin = -1;
    isActive = false;
    startTime = 0;
    duration = 0;
}

void BuzzerManager::init(int pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    
    Serial.println("Buzzer initialized");
}

void BuzzerManager::update() {
    if (isActive && (millis() - startTime > duration)) {
        stopTone();
    }
}

void BuzzerManager::beep(int durationMs) {
    playTone(1000, durationMs);
}

void BuzzerManager::beepSuccess() {
    playTone(2000, 100);
    delay(50);
    playTone(2500, 150);
}

void BuzzerManager::beepError() {
    playTone(500, 200);
    delay(100);
    playTone(500, 200);
}

void BuzzerManager::beepReady() {
    playTone(1500, 100);
}

void BuzzerManager::beepAcknowledge() {
    playTone(1800, 80);
}

void BuzzerManager::playTone(int frequency, int durationMs) {
    if (pin == -1) return;
    
    ledcSetup(0, frequency, 8);
    ledcAttachPin(pin, 0);
    ledcWrite(0, 128);  // 50% duty cycle
    
    isActive = true;
    startTime = millis();
    duration = durationMs;
}

void BuzzerManager::stopTone() {
    if (pin == -1) return;
    
    ledcWrite(0, 0);
    isActive = false;
}
