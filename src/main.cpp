// debug defines
#define ARDUINOHA_DEBUG

#define DATA_PIN_LED_INTERNAL 2
#define NUM_LEDS_INTERNAL 1
#define MAC_ADDRESS_SIZE 6
#define DEVICE_IDENTIFIER_SUFFIX_LENGTH 8

#include <Arduino.h>
#include <WiFiUdp.h>

#include <WiFi.h>
//#include <NTPClient.h>
#include <ArduinoHA.h>
#include <ESPmDNS.h>
#include <time.h>

#include <Preferences.h>

#include "DisplayLED.h"
#include "MQTTManager.h"
#include "OTAManager.h"
#include "WebserverManager.h"
#include "SettingsManager.h"
#include "WifiManager.h"
// Adafruit_NeoPixel internalLED(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

WiFiUDP ntpUDP;

int delayTime=10;

CRGB internalLed[NUM_LEDS_INTERNAL];
char deviceIdentifierSuffix[DEVICE_IDENTIFIER_SUFFIX_LENGTH];

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
//NTPClient timeClient(ntpUDP, 3600);
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
struct tm timeinfo;


void setup()
{
  // determine device ID by using the last two bytes of the mac address
  byte macAddress[MAC_ADDRESS_SIZE];
  WiFi.macAddress(macAddress);
  snprintf(deviceIdentifierSuffix,DEVICE_IDENTIFIER_SUFFIX_LENGTH,"0b%x%x",macAddress[MAC_ADDRESS_SIZE-2],macAddress[MAC_ADDRESS_SIZE-1]);
  
  
  Serial.begin(115200);
  delay(500);
  
  Serial.println("FakeNixie setup");

  DisplayLED.setup();
  DisplayLED.clear();
  SettingsManager.setup(deviceIdentifierSuffix);
  SettingsManager.load();
  SettingsManager.printToSerial();

  FastLED.addLeds<NEOPIXEL, DATA_PIN_LED_INTERNAL>(internalLed, NUM_LEDS_INTERNAL);
  //FastLED.setCorrection(TypicalSMD5050);
  FastLED.setCorrection(TypicalSMD5050);
  internalLed[0] = CRGB::Red;

  FastLED.show();


  WifiManager.begin();
  switch (WifiManager.getMode())
  {
  case WIFIMANAGER_MODE_AP:
    internalLed[0] = CRGB::Blue;
    break;
  case WIFIMANAGER_MODE_CONNECTED:
    internalLed[0] = CRGB::Green;
    OTAManager.setup();
    MQTTManager.setup(macAddress,MAC_ADDRESS_SIZE);
    MQTTManager.connect(S_MQTT_ADDR, S_MQTT_USER, S_MQTT_PASS);


  
    break;
  default:
    break;
  }

  FastLED.show();
  // Init and get the time
  configTime(0, 0, ntpServer);
  setenv("TZ",S_TIMEZONE.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
  

  

  //timeClient.begin();
  WebserverManager.setup();
}

void loop()
{
  getLocalTime(&timeinfo);
  int hours = timeinfo.tm_hour;
  int minutes = timeinfo.tm_min;
  int seconds = timeinfo.tm_sec;

  switch (WifiManager.getMode())
  {
  case WIFIMANAGER_MODE_AP:
    internalLed[0] = CRGB::Blue;
    DisplayLED.clear();
    DisplayLED.SetRGB(CRGB::Red);
    DisplayLED.SetBrightness(255);
    DisplayLED.SetPasscode(WifiManager.getApPasscode());
    break;
  case WIFIMANAGER_MODE_CONNECTED:
    internalLed[0] = CRGB::Green;
    MQTTManager.tick();
    OTAManager.tick();
    //timeClient.update();
  
    // Serial.println(timeClient.getFormattedTime());

    DisplayLED.SetTimeHMS(hours, minutes, seconds);
    break;
  default:
    break;
  }

  DisplayLED.update(delayTime);
  FastLED.show();

  delay(delayTime);
}
