#include <FastLED.h>
#include <DisplayLED.h>
#include <bits/stdc++.h>
#define DATA_PIN_LED_CLOCK 1
#define NUM_LEDS_CLOCK 120

// Define the array of leds
CRGB nixieLed[NUM_LEDS_CLOCK];

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
    
    SingleColor= CRGB::DarkOrange;
    LeftColor=CRGB::Blue;
    RightColor= CRGB::Red;
    LightMode=LIGHT_MODE_SINGLE;
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
        halfDigitColor_[i]= color;
    }
}

void DisplayLED_::SetRGB(CRGB leftColor,CRGB rightColor)
{
    Serial.print("leftColor: ");
    PrintColorToSerial(leftColor);
    Serial.print("rightColor: ");
    PrintColorToSerial(rightColor);
    fract8 fracPart=255/(NUM_HALF_DIGIT-1);
    for (uint8_t i = 0; i < NUM_HALF_DIGIT; i++)
    {

        halfDigitColor_[i]=halfDigitColor_[i].blend(leftColor,rightColor,(fract8)i*fracPart);
        PrintColorToSerial(halfDigitColor_[i]);
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
        SetRGB(LeftColor,RightColor);
        break;
    case LIGHT_MODE_RAINBOW:
        break;
    default:
        break;
    }
}

void DisplayLED_::PrintColorToSerial(CRGB color){
    Serial.println("R: "+String(color.r) + " G: "+ String(color.g) + " B: "+ String(color.b));

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
        if (i == digitOffset[value]  )
        {
            nixieLed[i + baseLedIdx] = GetRGB(digit);
        }
        else if ((i == (digitOffset[value] + LED_DIGIT_OFFSET)))
        {
            nixieLed[i + baseLedIdx] = GetRGB(digit+1);
        }
        else
        {
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
    Serial.print("set Color Temperature: ");
    Serial.print((int32_t)(temperature*100));
    Serial.print(", R: ");
    Serial.print(tempCol.r);
    Serial.print(", G: ");
    Serial.print(tempCol.g);
    Serial.print(", B: ");
    Serial.println(tempCol.b);
    return tempCol;
}

void DisplayLED_::SetColorTemperature(uint16_t temperature)
{
    CRGB tempCol=Mired2RGB(temperature);
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