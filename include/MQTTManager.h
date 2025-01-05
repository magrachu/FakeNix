#ifndef MQTTManager_h
#define MQTTManager_h
#include <Arduino.h>
#include <ArduinoHA.h>
#include <FastLED.h>
#define MQTT_STRING_LENGTH 256

class MQTTManager_
{
private:
    MQTTManager_() = default;

public:
    static MQTTManager_ &getInstance();
    HALight::RGBColor crgb2HArgb(CRGB color);
    void setup(byte* uniqueId, uint8_t uidLength);
    void connect(String servername, String username, String password);
    void printStatus();
    void tick();
};

extern MQTTManager_ &MQTTManager;

#endif
