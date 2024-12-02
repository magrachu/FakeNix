#include <MQTTManager.h>
#include <ArduinoHA.h>
#include <DisplayLED.h>
#include <WiFi.h>
#include <HAMqtt.h>

WiFiClient espWifiClient;

HADevice device("Elekstube-R");
HAMqtt mqtt(espWifiClient, device);
HALight light("digitLight", HALight::BrightnessFeature | HALight::ColorTemperatureFeature | HALight::RGBFeature);
int tickCounter = 0;

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
  CRGB val;
  val.setRGB(color.red, color.green, color.blue);
  DisplayLED.SetRGB(val);
  sender->setRGBColor(color); // report color back to the Home Assistant
}

void MQTTManager_::setup()
{
#warning "set constant for platform"
  byte mac[6];
  WiFi.macAddress(mac);
  String uniqueId = "Elekstube-" + String(mac[2]) + String(mac[3]) + String(mac[4]) + String(mac[5]);
  byte chUniqueId[50] = {69, 108, 101, 107, 's', 't', 'u', 'b', 'e', '-'};
  for (int i = 0; i < 4; i++)
  {
    chUniqueId[10 + i] = mac[2 + i];
  }
  chUniqueId[14] = 0;

  mqtt.setDiscoveryPrefix("homeassistant");
  device.enableExtendedUniqueIds();
  device.setUniqueId(mac, sizeof(mac));
  device.setName("Fake Nixie Clock");
  device.setSoftwareVersion("0.0.1");
  device.setManufacturer("magrachu");
  device.setModel("Elekstube-R ESP32");
  device.setConfigurationUrl("http://192.168.2.55:1234");
  Serial.println(device.getUniqueId());
  device.enableSharedAvailability();
  device.enableLastWill();

  light.onStateCommand(onStateCommand);
  light.onBrightnessCommand(onBrightnessCommand);             // optional
  light.onColorTemperatureCommand(onColorTemperatureCommand); // optional
  light.onRGBColorCommand(onRGBColorCommand);                 // optional
}

void MQTTManager_::connect(String servername, String username, String password)
{
  
  Serial.print("Connect to: ");
  Serial.println(servername.c_str());
  mqtt.begin(servername.c_str(), username.c_str(), password.c_str());
}

void MQTTManager_::tick()
{
  mqtt.loop();
  tickCounter += 1;
  if (tickCounter > 100)
  {
    tickCounter = 0;
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
}

