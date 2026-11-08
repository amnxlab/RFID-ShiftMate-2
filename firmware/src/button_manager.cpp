#include "button_manager.h"

ButtonManager::ButtonManager() {
    upPin = -1;
    downPin = -1;
    selectPin = -1;
}

void ButtonManager::init(int upPin, int downPin, int selectPin) {
    this->upPin = upPin;
    this->downPin = downPin;
    this->selectPin = selectPin;
    
    pinMode(upPin, INPUT_PULLUP);
    pinMode(downPin, INPUT_PULLUP);
    pinMode(selectPin, INPUT_PULLUP);
    
    btnUp.attach(upPin);
    btnUp.interval(25);
    
    btnDown.attach(downPin);
    btnDown.interval(25);
    
    btnSelect.attach(selectPin);
    btnSelect.interval(25);
    
    Serial.println("Buttons initialized");
}

void ButtonManager::update() {
    btnUp.update();
    btnDown.update();
    btnSelect.update();
}

bool ButtonManager::wasPressed(int pin) {
    if (pin == upPin) return btnUp.fell();
    if (pin == downPin) return btnDown.fell();
    if (pin == selectPin) return btnSelect.fell();
    return false;
}

bool ButtonManager::isPressed(int pin) {
    if (pin == upPin) return !digitalRead(upPin);
    if (pin == downPin) return !digitalRead(downPin);
    if (pin == selectPin) return !digitalRead(selectPin);
    return false;
}

bool ButtonManager::wasReleased(int pin) {
    if (pin == upPin) return btnUp.rose();
    if (pin == downPin) return btnDown.rose();
    if (pin == selectPin) return btnSelect.rose();
    return false;
}
