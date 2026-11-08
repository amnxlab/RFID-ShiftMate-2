#include "ethernet_manager.h"
#include "config.h"

bool EthernetManager::ethConnected = false;
IPAddress EthernetManager::currentIP;

EthernetManager::EthernetManager() {
    initialized = false;
    lastCheck = 0;
}

bool EthernetManager::init() {
    Serial.println("Initializing Ethernet (W5500)...");
    
    // Configure SPI for W5500
    SPI.begin(ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN, ETH_SS_PIN);
    
    // Initialize Ethernet
    ETH.begin(
        ETH_ADDR,           // ETH_ADDR (usually 0 or 1)
        ETH_INT_PIN,        // INT pin (or -1 if not used)
        ETH_CS_PIN,         // CS pin
        SPI,                // SPI instance
        ETH_CLK_MHZ,        // SPI clock speed
        ETH_PHY_TYPE        // PHY type (W5500)
    );
    
    // Wait for connection
    unsigned long start = millis();
    while (!isConnected() && (millis() - start) < 10000) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    
    if (isConnected()) {
        initialized = true;
        currentIP = ETH.localIP();
        Serial.print("Ethernet connected. IP: ");
        Serial.println(currentIP);
        return true;
    }
    
    return false;
}

bool EthernetManager::isConnected() {
    return ethConnected && (ETH.linkStatus() == ETH_LINK_UP);
}

IPAddress EthernetManager::getLocalIP() {
    return currentIP;
}

String EthernetManager::getMACAddress() {
    uint8_t mac[6];
    ETH.macAddress(mac);
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

void EthernetManager::update() {
    if (millis() - lastCheck > 1000) {
        lastCheck = millis();
        
        if (isConnected()) {
            currentIP = ETH.localIP();
        }
    }
}

void EthernetManager::ethEventCallback(int event) {
    switch (event) {
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            ETH.setHostname("rfid-shiftmate-2");
            break;
            
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
            
        case ARDUINO_EVENT_ETH_GOT_IP:
            ethConnected = true;
            currentIP = ETH.localIP();
            Serial.print("Ethernet got IP: ");
            Serial.println(currentIP);
            break;
            
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            ethConnected = false;
            Serial.println("Ethernet disconnected");
            break;
            
        case ARDUINO_EVENT_ETH_STOP:
            ethConnected = false;
            Serial.println("Ethernet stopped");
            break;
    }
}
