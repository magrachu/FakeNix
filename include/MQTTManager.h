#ifndef MQTTManager_h
#define MQTTManager_h
#include <Arduino.h>

class MQTTManager_
{
private:
    MQTTManager_() = default;

public:
    static MQTTManager_ &getInstance();
    void setup();
    void connect(String servername, String username, String password);
    void tick();
};

extern MQTTManager_ &MQTTManager;

#endif
