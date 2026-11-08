#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <Arduino.h>

class BuzzerManager {
public:
    BuzzerManager();
    
    void init(int pin);
    void update();
    
    void beep(int durationMs);
    void beepSuccess();
    void beepError();
    void beepReady();
    void beepAcknowledge();
    
private:
    int pin;
    bool isActive;
    unsigned long startTime;
    int duration;
    
    void playTone(int frequency, int durationMs);
    void stopTone();
};

#endif // BUZZER_MANAGER_H
