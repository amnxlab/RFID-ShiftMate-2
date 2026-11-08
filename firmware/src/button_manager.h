#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include <Bounce2.h>

class ButtonManager {
public:
    ButtonManager();
    
    void init(int upPin, int downPin, int selectPin);
    void update();
    
    bool wasPressed(int pin);
    bool isPressed(int pin);
    bool wasReleased(int pin);
    
private:
    Bounce btnUp;
    Bounce btnDown;
    Bounce btnSelect;
    
    int upPin;
    int downPin;
    int selectPin;
};

#endif // BUTTON_MANAGER_H
