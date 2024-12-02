#ifndef WifiManager_h
#define WifiManager_h
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <SettingsManager.h>


enum WifiMode {WIFIMANAGER_MODE_IDLE, WIFIMANAGER_MODE_BUSY, WIFIMANAGER_MODE_AP, WIFIMANAGER_MODE_CONNECTED};

class WifiManager_
{
private:
    WifiManager_() =default;
    WifiMode currentWifiMode, targetWifiMode;
public:
    WifiManager_ &getInstance();
    void setup();
    void begin();
    WifiMode getMode();
    int getApPasscode();
    int16_t getNumNetworks();
    String getNetworkInfoString(uint8_t num);


};
extern WifiManager_ &WifiManager;




#endif