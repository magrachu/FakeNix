#include <FastLED.h>
#include <DisplayLED.h>

#define DATA_PIN_LED_CLOCK 1
#define NUM_LEDS_CLOCK 120

// Define the array of leds
CRGB nixieLed[NUM_LEDS_CLOCK];

// led order is 1, 3, 5, 7, 9, 0, 2, 4, 6, 8
const int digitOffset[10] = {5, 0, 6, 1, 7, 2, 8, 3, 9, 4};
#define LED_DIGIT_OFFSET 10
#define LED_PER_DIGIT 2

bool state_ = true;
CRGB baseColor_ = CRGB::DarkOrange;
uint8_t brightness_ = 255;

DisplayLED_ &DisplayLED_::getInstance()
{
    static DisplayLED_ instance;
    return instance;
}

void DisplayLED_::setup()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN_LED_CLOCK>(nixieLed, NUM_LEDS_CLOCK);
}

void DisplayLED_::SetState(bool state)
{
    state_ = state;
}
void DisplayLED_::SetRGB(CRGB color)
{
    baseColor_ = color;
}
void DisplayLED_::SetBrightness(uint8_t brightness)
{
    brightness_ = brightness;
}

CRGB DisplayLED_::getRGB()
{
    CRGB col;
    if (state_ == true)
    {
        col = baseColor_;
        col.maximizeBrightness();
        col.nscale8_video(brightness_);
        return col;
    }
    else
    {
        return CRGB::Black;
    }
}

void DisplayLED_::SetDigit(int digit, int value)
{
    int baseLedIdx = digit * LED_DIGIT_OFFSET * LED_PER_DIGIT;
    for (int i = 0; i < LED_DIGIT_OFFSET * LED_PER_DIGIT; i++)
    {
        // check array size first
        if (i == digitOffset[value] || (i == (digitOffset[value] + LED_DIGIT_OFFSET)))
        {
            nixieLed[i + baseLedIdx] = getRGB();
        }
        else
        {
            nixieLed[i + baseLedIdx].nscale8(200);
            // leds[i+baseLedIdx].subtractFromRGB(clockColor.r/10);
            //  leds[i+baseLedIdx]-=clockColor/10;
        }
    }

    // led order is 1a, 3a... 9a, 0a, 2a ... 8a, 1b, 3b... 9b, 0b, 2b ... 8b
}

int DisplayLED_::get2Digit(int value, int digit)
{
    if (digit == 0)
        return (value % 10);
    else
        return (value / 10);
}

ColorTemperature DisplayLED_::getColorTemperature(uint16_t temperature)
{
    // HA reports color temperature in Mired
    if (temperature < 140)
        return ColorTemperature::DirectSunlight;
    if (temperature < 170)
        return ColorTemperature::HighNoonSun;
    if (temperature < 180)
        return ColorTemperature::CarbonArc;
    if (temperature < 190)
        return ColorTemperature::Halogen;
    if (temperature < 230)
        return ColorTemperature::Tungsten100W;
    if (temperature < 340)
        return ColorTemperature::Tungsten40W;
    if (temperature < 380)
        return ColorTemperature::Candle;
    // default return value if outside range
    return ColorTemperature::Candle;
}

void DisplayLED_::SetColorTemperature(uint16_t temperature)
{
    baseColor_.setColorCode(getColorTemperature(temperature));
}

void DisplayLED_::SetTimeHMS(int hours, int minutes, int seconds)
{
    SetDigit(0, get2Digit(hours, 1));
    SetDigit(1, get2Digit(hours, 0));
    SetDigit(2, get2Digit(minutes, 1));
    SetDigit(3, get2Digit(minutes, 0));
    SetDigit(4, get2Digit(seconds, 1));
    SetDigit(5, get2Digit(seconds, 0));
}

void DisplayLED_::SetPasscode(int passcode)
{
    int temp= passcode;
    for (uint8_t i = 0; i < 6; i++)
    {
        SetDigit(5-i,temp % 10);
        temp/=10;
    }
    

}

void DisplayLED_::update()
{
}

void DisplayLED_::clear()
{
    for (int i = 0; i < NUM_LEDS_CLOCK; i++)
    {
        nixieLed[i] = CRGB::Black;
    }
    
    
}

DisplayLED_ &DisplayLED = DisplayLED.getInstance();