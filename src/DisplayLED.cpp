#include <FastLED.h>
#include <DisplayLED.h>
#include <bits/stdc++.h>
#define DATA_PIN_LED_CLOCK 1
#define NUM_LEDS_CLOCK 120

// Define the array of leds
CRGB nixieLed[NUM_LEDS_CLOCK];
float rainbowFraction = 0;

CHSV currentRainbowColorHSV[2];
CRGB currentRainbowColorRGB[2];
// led order is 1, 3, 5, 7, 9, 0, 2, 4, 6, 8
const int digitOffset[10] = {5, 0, 6, 1, 7, 2, 8, 3, 9, 4};
#define LED_DIGIT_OFFSET 10
#define LED_PER_DIGIT 2
#define NUM_HALF_DIGIT 12

bool state_ = true;
// CRGB baseColor_ = CRGB::DarkOrange;
CRGB halfDigitColor_[NUM_HALF_DIGIT];

uint8_t brightness_ = 255;

DisplayLED_ &DisplayLED_::getInstance()
{
    static DisplayLED_ instance;
    return instance;
}

void DisplayLED_::setup()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN_LED_CLOCK>(nixieLed, NUM_LEDS_CLOCK);

    SingleColor = CRGB::DarkOrange;
    LeftColor = CRGB::Blue;
    RightColor = CRGB::Red;
    LightMode = LIGHT_MODE_SINGLE;

    currentRainbowColorHSV[0].setHSV(0, 255, 255);
    UpdateColor();
}

void DisplayLED_::SetState(bool state)
{
    state_ = state;
}
void DisplayLED_::SetRGB(CRGB color)
{
    for (uint8_t i = 0; i < NUM_HALF_DIGIT; i++)
    {
        halfDigitColor_[i] = color;
    }
}

void DisplayLED_::SetRGB(CRGB leftColor, CRGB rightColor)
{
    // in order to do a proper spectrum blend, we need to convert to HSV first
    CHSV color[2];
    color[0]=rgb2hsv_approximate(leftColor);
    color[1]=rgb2hsv_approximate(rightColor);
    CHSV mixHSV;
    CRGB mixRGB;
    //  Serial.print("leftColor: ");
    //  PrintColorToSerial(leftColor);
    //  Serial.print("rightColor: ");
    //  PrintColorToSerial(rightColor);
    fract8 fracPart = 255 / (NUM_HALF_DIGIT - 1);
    for (uint8_t i = 0; i < NUM_HALF_DIGIT; i++)
    {
        mixHSV=blend(color[0],color[1],(fract8)i * fracPart,FORWARD_HUES);
        mixRGB= hsv2rgb_spectrum(mixHSV);
        //halfDigitColor_[i] = halfDigitColor_[i].blend(leftColor, rightColor, (fract8)i * fracPart);
        halfDigitColor_[i]=hsv2rgb_spectrum(mixHSV);
        //  PrintColorToSerial(halfDigitColor_[i]);
    }
}

void DisplayLED_::UpdateColor()
{
    switch (LightMode)
    {
    case LIGHT_MODE_SINGLE:
        SetRGB(SingleColor);
        break;
    case LIGHT_MODE_GRADIENT:
        SetRGB(LeftColor, RightColor);
        break;
    case LIGHT_MODE_RAINBOW:
        break;
    default:
        break;
    }
}

void DisplayLED_::PrintColorToSerial(CRGB color)
{
    Serial.println("R: " + String(color.r) + " G: " + String(color.g) + " B: " + String(color.b));
}

void DisplayLED_::SetBrightness(uint8_t brightness)
{
    brightness_ = brightness;
    UpdateColor();
}

CRGB DisplayLED_::GetRGB(uint8_t halfDigitIndex)
{
    CRGB col;
    if (state_ == true)
    {
        col = halfDigitColor_[halfDigitIndex];
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
        if (i == digitOffset[value])
        {
            nixieLed[i + baseLedIdx] = GetRGB(2*digit);
        }
        else if ((i == (digitOffset[value] + LED_DIGIT_OFFSET)))
        {
            nixieLed[i + baseLedIdx] = GetRGB(2*digit + 1);
        }
        else
        {
            // the digit should fade out
            nixieLed[i + baseLedIdx].nscale8(200);
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

CRGB DisplayLED_::Mired2RGB(uint16_t mired)
{
    // use the algorithm from
    // https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html
    // which basically approximates the curves you get from this chart data
    // http://www.vendian.org/mncharity/dir3/blackbody/UnstableURLs/bbr_color.html

    // homeassistant reports colortemperature in Mired, so convert to 100s of kelvins first

    float temperature = 10000.0 / ((float)(mired));
    CRGB tempCol = CRGB::White;
    float redVal, greenVal, blueVal;
    const float minVal = 00;
    const float maxVal = 255.0;
    // the algorithm can be simplified by truncating at 6600K (RGB #FFFFFF) as HA does not go above that value.
    if (temperature < 66)
    {

        tempCol.r = 255;

        greenVal = 99.4708025861 * log(temperature) - 161.1195681661;
        tempCol.g = min(maxVal, max(minVal, greenVal));

        if (temperature <= 19)
        {
            tempCol.b = 0;
        }
        else
        {
            blueVal = temperature - 10;
            blueVal = 138.5177312231 * log(blueVal) - 305.0447927307;
            tempCol.b = min(maxVal, max(minVal, blueVal));
        }
    }
    // Serial.print("set Color Temperature: ");
    // Serial.print((int32_t)(temperature * 100));
    // Serial.print(", R: ");
    // Serial.print(tempCol.r);
    // Serial.print(", G: ");
    // Serial.print(tempCol.g);
    // Serial.print(", B: ");
    // Serial.println(tempCol.b);
    return tempCol;
}

void DisplayLED_::SetColorTemperature(uint16_t temperature)
{
    CRGB tempCol = Mired2RGB(temperature);
    SetRGB(tempCol);
    // for (uint8_t i = 0; i < NUM_HALF_DIGIT; i++)
    // {
    //     halfDigitColor_[i]= tempCol;
    // }
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
    int temp = passcode;
    for (uint8_t i = 0; i < 6; i++)
    {
        SetDigit(5 - i, temp % 10);
        temp /= 10;
    }
}

void DisplayLED_::update(int interval)
{
    // include all the functions in here and not in setTime members
    double wheelIncrement;
    double timeIncrement = (double)interval / 1000.0;
    // Serial.println(wheelIncrement);
    switch (LightMode)
    {
    case LIGHT_MODE_SINGLE:
        break;
    case LIGHT_MODE_GRADIENT:
        break;
    case LIGHT_MODE_RAINBOW:
        // calculate rainbow fraction
        // interval comes in milliseconds

        // 255=rainbowDuration*rainbowSpeed
        // rainbowSpeed= 255/rainbowDuration, that is in increments per second
        if (rainbowDuration > 0)
        {
            wheelIncrement = 255.0 / rainbowDuration * timeIncrement;
        }
        else
        {
            wheelIncrement = 0.1;
        }

        rainbowFraction += wheelIncrement;
        if (rainbowFraction > 255.0)
        {
            rainbowFraction = 0;
        }
        currentRainbowColorHSV[0].h = (uint8_t)rainbowFraction;
        currentRainbowColorHSV[1] = currentRainbowColorHSV[0];
        currentRainbowColorHSV[1].h += rainbowOffset;
        hsv2rgb_rainbow(currentRainbowColorHSV[0], currentRainbowColorRGB[0]);
        hsv2rgb_rainbow(currentRainbowColorHSV[1], currentRainbowColorRGB[1]);
        SetRGB(currentRainbowColorRGB[0], currentRainbowColorRGB[1]);
        break;
    default:
        break;
    }
}

void DisplayLED_::clear()
{
    for (int i = 0; i < NUM_LEDS_CLOCK; i++)
    {
        nixieLed[i] = CRGB::Black;
    }
}

DisplayLED_ &DisplayLED = DisplayLED.getInstance();