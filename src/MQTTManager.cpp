#include <MQTTManager.h>

#include <DisplayLED.h>
#include <WiFi.h>
#include <SettingsManager.h>
#include <ESPmDNS.h>

WiFiClient espWifiClient;

HADevice device;
HAMqtt mqtt(espWifiClient, device);
HALight light("digitLight", HALight::BrightnessFeature | HALight::ColorTemperatureFeature | HALight::RGBFeature);
HALight lightLeft("leftSideColor", HALight::BrightnessFeature | HALight::RGBFeature);
HALight lightRight("rightSideColor", HALight::BrightnessFeature | HALight::RGBFeature);
HASelect lightMode("lightMode");
HANumber rainbowDuration("RainbowDuration", HANumber::PrecisionP2);
HANumber rainbowOffset("RainbowOffset", HANumber::PrecisionP0);

int tickCounter = 0;
char _username[MQTT_STRING_LENGTH];
char _password[MQTT_STRING_LENGTH];
char _hostname[MQTT_STRING_LENGTH];

char _configURL[MQTT_STRING_LENGTH] = "http://";

MQTTManager_ &MQTTManager_::getInstance()
{
  static MQTTManager_ instance;
  return instance;
}

MQTTManager_ &MQTTManager = MQTTManager.getInstance();

HALight::RGBColor crgb2HArgb(CRGB color){
  HALight::RGBColor col;
  col.red=color.r;
  col.green=color.g;
  col.blue=color.b;
  return col;
  
}

void onStateCommand(bool state, HALight *sender)
{
  
  if (sender== &light && DisplayLED.LightMode == LIGHT_MODE_SINGLE)
  {
    if (state == 0)
      DisplayLED.SetState(false);
    else
      DisplayLED.SetState(true);
  }
  if (sender->getObjectId() == lightLeft.getObjectId())
  {
    // do nothing
  }
  if (sender->getObjectId() == lightRight.getObjectId())
  {
    // do nothing
  }

  sender->setState(state); // report state back to the Home Assistant
}

void onBrightnessCommand(uint8_t brightness, HALight *sender)
{
  if (sender== &light)
  {
    DisplayLED.SetBrightness(brightness);
    
    DisplayLED.UpdateColor();
  }
  sender->setBrightness(brightness); // report brightness back to the Home Assistant
}

void onColorTemperatureCommand(uint16_t temperature, HALight *sender)
{
  if (sender== &light)
  {
    DisplayLED.SetColorTemperature(temperature);
  sender->setColorTemperature(temperature); // report color temperature back to the Home Assistant
  }
  
  
}

void onRGBColorCommand(HALight::RGBColor color, HALight *sender)
{
  CRGB val;
  val.setRGB(color.red, color.green, color.blue);

  if (sender== &light)
  {
    Serial.print("Set Single color: ");
    DisplayLED.PrintColorToSerial(val);
    DisplayLED.SingleColor = val;
    DisplayLED.UpdateColor();
  }
  if (sender== &lightLeft)
  {
    Serial.print("Set Left color: ");
    DisplayLED.PrintColorToSerial(val);
    DisplayLED.LeftColor = val;
    DisplayLED.UpdateColor();
  }
  if (sender== &lightRight)
  {
    Serial.print("Set Right color: ");
    DisplayLED.PrintColorToSerial(val);
    DisplayLED.RightColor = val;
    DisplayLED.UpdateColor();
  }

  sender->setRGBColor(color); // report color back to the Home Assistant
}

void onSelectCommand(int8_t index, HASelect *sender)
{
  switch (index)
  {
  case 0:
    DisplayLED.LightMode = LIGHT_MODE_SINGLE;
    DisplayLED.UpdateColor();
    break;
  case 1:
    DisplayLED.LightMode = LIGHT_MODE_GRADIENT;
    DisplayLED.UpdateColor();
    break;

  case 2:
    DisplayLED.LightMode = LIGHT_MODE_RAINBOW;
    DisplayLED.UpdateColor();
    break;

  default:
    DisplayLED.LightMode = LIGHT_MODE_SINGLE;
    DisplayLED.UpdateColor();
    // unknown option
    return;
  }

  sender->setState(index); // report the selected option back to the HA panel

  // it may return null
  if (sender->getCurrentOption())
  {
    Serial.print("Current option: ");
    Serial.println(sender->getCurrentOption());
  }
}

void onNumberCommand(HANumeric number, HANumber* sender)
{
    if (!number.isSet()) {
        // the reset command was send by Home Assistant
    } else {
      if (sender == &rainbowDuration)
      {
        DisplayLED.rainbowDuration=number.toFloat();
      }
      if (sender == &rainbowOffset)
      {
        DisplayLED.rainbowOffset=(uint8_t)((float)255*number.toFloat()/360);
      }
      
   
    }

    sender->setState(number); // report the selected option back to the HA panel
}

void MQTTManager_::setup(byte *uniqueId, uint8_t uidLength)
{


  device.setUniqueId(uniqueId, uidLength);

  mqtt.setDataPrefix(S_MQTT_DATA_PREFIX.c_str());
  mqtt.setDiscoveryPrefix(S_MQTT_HA_DISCOVERY_PREFIX.c_str());
  device.enableExtendedUniqueIds();

  device.setName("Fake Nixie Clock");
  device.setSoftwareVersion("0.0.1");
  device.setManufacturer("magrachu");
  device.setModel("FakeNixie");

  device.enableSharedAvailability();
  device.enableLastWill();
  light.setObjectId("Single color");

  light.setName("Single Light");
  lightLeft.setName("Left side color");
  // lightLeft.setIcon("mdi:menu-left");
  lightRight.setName("Right side color");
  // lightRight.setIcon("mdi:menu-right");

  light.onStateCommand(onStateCommand);
  light.onBrightnessCommand(onBrightnessCommand);
  light.onColorTemperatureCommand(onColorTemperatureCommand);
  light.onRGBColorCommand(onRGBColorCommand);
light.setOptimistic(true);
lightLeft.onStateCommand(onStateCommand);
  lightLeft.onBrightnessCommand(onBrightnessCommand);
  lightLeft.onColorTemperatureCommand(onColorTemperatureCommand);
  lightLeft.onRGBColorCommand(onRGBColorCommand);

  lightRight.onStateCommand(onStateCommand);
  lightRight.onBrightnessCommand(onBrightnessCommand);
  lightRight.onColorTemperatureCommand(onColorTemperatureCommand);
  lightRight.onRGBColorCommand(onRGBColorCommand);

  light.setCurrentBrightness(255);
  light.setCurrentState(true);

  lightLeft.setCurrentBrightness(255);
  lightLeft.setCurrentState(true);


  lightRight.setCurrentBrightness(255);
  lightRight.setCurrentState(true);
 

  light.setCurrentRGBColor(HALight::RGBColor(DisplayLED.SingleColor.r, DisplayLED.SingleColor.g, DisplayLED.SingleColor.b));
  lightLeft.setCurrentRGBColor(HALight::RGBColor(DisplayLED.LeftColor.r, DisplayLED.LeftColor.g, DisplayLED.LeftColor.b));
  lightRight.setCurrentRGBColor(HALight::RGBColor(DisplayLED.RightColor.r, DisplayLED.RightColor.g, DisplayLED.RightColor.b));

  // set available options
  lightMode.setOptions("Single;Gradient;Rainbow"); // use semicolons as separator of options
  lightMode.onCommand(onSelectCommand);
  lightMode.setCurrentState(0);
  lightMode.setIcon("mdi:home");   // optional
  lightMode.setName("Light mode"); // optional

  rainbowDuration.setName("Rainbow duration");
  rainbowDuration.setMode(HANumber::ModeSlider);
  rainbowDuration.setCurrentState((float)10.0);
  rainbowDuration.setMin(0.1);
  rainbowDuration.setMax(100);
  rainbowDuration.setStep(0.1);
rainbowDuration.setUnitOfMeasurement("s");
  rainbowDuration.onCommand(onNumberCommand);
rainbowOffset.setName("Rainbow offset");
  rainbowOffset.setMode(HANumber::ModeSlider);
  //rainbowOffset.setCurrentState((int)0);
  rainbowOffset.setStep(1);
  rainbowOffset.setMin(0);
  rainbowOffset.setMax(360);
  rainbowOffset.setUnitOfMeasurement("°");

  rainbowOffset.onCommand(onNumberCommand);
}

void MQTTManager_::connect(String servername, String username, String password)
{

  // copy the credentials into char arrays locally because the arduinoHA library does only reference them
  strcpy(_hostname, servername.c_str());
  strcpy(_username, username.c_str());
  strcpy(_password, password.c_str());

  mqtt.begin(_hostname, _username, _password);

  //MDNS.begin(_mdnsHostname);
  HALight::RGBColor val;
//   light.setRGBColor(HALight::RGBColor(DisplayLED.SingleColor.r, DisplayLED.SingleColor.g, DisplayLED.SingleColor.b));
//   lightLeft.setRGBColor(HALight::RGBColor(DisplayLED.LeftColor.r, DisplayLED.LeftColor.g, DisplayLED.LeftColor.b));
//   lightRight.setRGBColor(HALight::RGBColor(DisplayLED.RightColor.r, DisplayLED.RightColor.g, DisplayLED.RightColor.b));
//   lightMode.setState(0);
//   light.setBrightness(255);
//   light.setState(1);
//  lightLeft.setState(1);
//  lightLeft.setBrightness(255);
//   lightRight.setState(1);
//   lightRight.setBrightness(255);
   
}

void MQTTManager_::tick()
{
  mqtt.loop();
}

void MQTTManager_::printStatus()
{
  switch (mqtt.getState())
  {
  case -5:
    Serial.println("MQTT State: StateConnecting");
    break;
  case -4:
    Serial.println("MQTT State: StateConnectionTimeout");
    break;
  case -3:
    Serial.println("MQTT State: StateConnectionLost");
    break;
  case -2:
    Serial.println("MQTT State: StateConnectionFailed");
    break;
  case -1:
    Serial.println("MQTT State: StateDisconnected");
    break;
  case 0:
    Serial.println("MQTT State: StateConnected");
    break;
  case 1:
    Serial.println("MQTT State: StateBadProtocol");
    break;
  case 2:
    Serial.println("MQTT State: StateBadClientId");
    break;
  case 3:
    Serial.println("MQTT State: StateUnavailable");
    break;
  case 4:
    Serial.println("MQTT State: StateBadCredentials");
    break;
  case 5:
    Serial.println("MQTT State: StateUnauthorized");
  default:
    break;
  }
}
