#include <MQTTManager.h>
#include <ArduinoHA.h>
#include <DisplayLED.h>
#include <WiFi.h>
#include <SettingsManager.h>
#include <ESPmDNS.h>



WiFiClient espWifiClient;

HADevice device;
HAMqtt mqtt(espWifiClient, device);
HALight light("digitLight", HALight::BrightnessFeature | HALight::ColorTemperatureFeature | HALight::RGBFeature);
HALight lightLeft("leftSideColor",HALight::BrightnessFeature  | HALight::RGBFeature);
HALight lightRight("rightSideColor",HALight::BrightnessFeature  | HALight::RGBFeature);

int tickCounter = 0;
char _username[MQTT_STRING_LENGTH];
char _password[MQTT_STRING_LENGTH];
char _hostname[MQTT_STRING_LENGTH];
char _configURL[MQTT_STRING_LENGTH]= "http://";

MQTTManager_ &MQTTManager_::getInstance()
{
  static MQTTManager_ instance;
  return instance;
}

MQTTManager_ &MQTTManager = MQTTManager.getInstance();

void onStateCommand(bool state, HALight *sender)
{
  if (state == 0)
    DisplayLED.SetState(false);
  else
    DisplayLED.SetState(true);
  sender->setState(state); // report state back to the Home Assistant
}

void onBrightnessCommand(uint8_t brightness, HALight *sender)
{
  DisplayLED.SetBrightness(brightness);
  sender->setBrightness(brightness); // report brightness back to the Home Assistant
}

void onColorTemperatureCommand(uint16_t temperature, HALight *sender)
{
  DisplayLED.SetColorTemperature(temperature);
  sender->setColorTemperature(temperature); // report color temperature back to the Home Assistant
}

void onRGBColorCommand(HALight::RGBColor color, HALight *sender)
{
  if (sender->getObjectId() == light.getObjectId())
  {
    CRGB val;
    val.setRGB(color.red, color.green, color.blue);
    DisplayLED.SetRGB(val);

  }
  if (sender->getObjectId() == light.getObjectId())
  {
    CRGB val;
    val.setRGB(color.red, color.green, color.blue);
    DisplayLED.SetRGB(val);
  }

  
  
  sender->setRGBColor(color); // report color back to the Home Assistant
}

void MQTTManager_::setup(byte* uniqueId, uint8_t uidLength)
{
 
 
  device.setUniqueId(uniqueId,uidLength);

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
  lightLeft.setIcon("mdi:menu-left");
  lightRight.setName("Right side color");
  lightRight.setIcon("mdi:menu-right");

  light.onStateCommand(onStateCommand);
  light.onBrightnessCommand(onBrightnessCommand);             
  light.onColorTemperatureCommand(onColorTemperatureCommand); 
  light.onRGBColorCommand(onRGBColorCommand);                 
}

void MQTTManager_::connect(String servername, String username, String password)
{

  // copy the credentials into char arrays locally because the arduinoHA library does only reference them
  strcpy(_hostname, servername.c_str());
  strcpy(_username, username.c_str());
  strcpy(_password, password.c_str());

  mqtt.begin(_hostname, _username, _password);


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
