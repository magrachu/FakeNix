#ifndef SettingsManager_h
#define SettingsManager_h
#include <Arduino.h>


class SettingsManager_
{
private:
    SettingsManager_() = default;

public:
    void setup(char* deviceIdentifierSuffix);
    void load();
    void save();
    void reset();
    void printToSerial();
    SettingsManager_ &getInstance();
};

void loadSettings();
extern SettingsManager_ &SettingsManager;

extern String S_WIFI_SSID;
extern String S_WIFI_PASS;
extern String S_MQTT_ADDR;
extern String S_MQTT_USER;
extern String S_MQTT_PASS;
extern String S_MQTT_DATA_PREFIX;
extern String S_MQTT_HA_DISCOVERY_PREFIX;
extern String S_MDNS_HOSTNAME;

#endif