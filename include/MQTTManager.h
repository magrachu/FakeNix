#ifndef MQTTManager_h
#define MQTTManager_h
#include <Arduino.h>

#define MQTT_STRING_LENGTH 256

class MQTTManager_
{
private:
    MQTTManager_() = default;

public:
    static MQTTManager_ &getInstance();
    void setup();
    void connect(String servername, String username, String password);
    void printStatus();
    void tick();
};

extern MQTTManager_ &MQTTManager;

#endif
