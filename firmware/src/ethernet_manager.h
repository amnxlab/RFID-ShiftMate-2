#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <Arduino.h>
#include <ETH.h>

class EthernetManager {
public:
    EthernetManager();
    
    bool init();
    bool isConnected();
    IPAddress getLocalIP();
    String getMACAddress();
    
    void update();
    
private:
    bool initialized;
    unsigned long lastCheck;
    
    static bool ethConnected;
    static IPAddress currentIP;
    
    static void ethEventCallback(int event);
};

#endif // ETHERNET_MANAGER_H
