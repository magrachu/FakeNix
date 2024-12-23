#ifndef DisplayLED_h
#define DisplayLED_h

#include <FastLED.h>
#include <ArduinoHA.h>

class DisplayLED_
{
private:
    DisplayLED_() = default;
    CRGB getRGB();
    int get2Digit(int value, int digit);
    void SetDigit(int digit, int value);


public:
    static DisplayLED_ &getInstance();
    void setup();

    void SetState(bool state);
    void SetRGB(CRGB color);
    void SetRGB(CRGB leftColor,CRGB rightColor);
    CRGB GetRGB(uint8_t halfDigitIndex);
    void SetBrightness(uint8_t brightness);
    CRGB Mired2RGB(uint16_t mired);
    void SetColorTemperature(uint16_t temperature);
    void SetTimeHMS(int hours, int minutes, int seconds);
    void SetPasscode(int passcode);
    void PrintColorToSerial(CRGB color);
    void clear();
    void update();
};
extern DisplayLED_ &DisplayLED;

#endif