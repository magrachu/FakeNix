#include "SettingsManager.h"
#include <Preferences.h>

Preferences Settings;
#define SETTINGS_NS "Elekstube-R"
#define RW_MODE false
#define RO_MODE true

// keys for preferences
#define KEY_INIT "nvsinit"
#define KEY_WIFI_SSID "wifissid"
#define KEY_WIFI_PASS "wifipass"
#define KEY_MQTT_ADDR "mqttaddr"
#define KEY_MQTT_USER "mqttuser"
#define KEY_MQTT_PASS "mqttpass"
#define KEY_MQTT_DATA_PREFIX "mqttdataprefix"
#define KEY_MQTT_HA_DISCOVERY_PREFIX "mqttdiscprefix"
#define KEY_MDNS_HOSTNAME "mdnshostname"

// default values for preferences
#define DEFAULT_WIFI_SSID "ssid"
#define DEFAULT_WIFI_PASS "password"
#define DEFAULT_MQTT_ADDR "homeassistant.local"
#define DEFAULT_MQTT_USER "username"
#define DEFAULT_MQTT_PASS "password"
#define DEFAULT_MQTT_DATA_PREFIX "fakenixie_"
#define DEFAULT_MQTT_HA_DISCOVERY_PREFIX "homeassistant"
#define DEFAULT_MDNS_HOSTNAME "fakenixie_"

SettingsManager_ &SettingsManager_::getInstance()
{
    static SettingsManager_ instance;
    return instance;
}

SettingsManager_ &SettingsManager = SettingsManager.getInstance();

void SettingsManager_::setup(char* deviceIdentifierSuffix)
{
    // check whether namespace exists, if not, create it
    Settings.begin(SETTINGS_NS, RO_MODE);

    bool tpInit = Settings.isKey(KEY_INIT);
    Settings.end();
    if (tpInit == false)
    {
        Serial.println("Initialize Settings");
        Settings.begin(SETTINGS_NS, RW_MODE);

        // write default values
        Settings.putInt(KEY_INIT,0);
        Settings.putString(KEY_WIFI_SSID, DEFAULT_WIFI_SSID);
        Settings.putString(KEY_WIFI_PASS, DEFAULT_WIFI_PASS);
        Settings.putString(KEY_MQTT_ADDR, DEFAULT_MQTT_ADDR);
        Settings.putString(KEY_MQTT_USER, DEFAULT_MQTT_USER);
        Settings.putString(KEY_MQTT_PASS, DEFAULT_MQTT_PASS);
        // use a device specific id for the data prefix and the hostname in case there are two devices to set up at the same time
        Settings.putString(KEY_MQTT_DATA_PREFIX, DEFAULT_MQTT_DATA_PREFIX);
        Settings.putString(KEY_MQTT_HA_DISCOVERY_PREFIX, DEFAULT_MQTT_HA_DISCOVERY_PREFIX);
        Settings.putString(KEY_MDNS_HOSTNAME, DEFAULT_MDNS_HOSTNAME);

        Settings.end();
    }
}

void SettingsManager_::load()
{
    Settings.begin(SETTINGS_NS, RO_MODE);
    S_WIFI_SSID = Settings.getString(KEY_WIFI_SSID, DEFAULT_WIFI_SSID);
    Serial.println("Settings load SSID: "+String(S_WIFI_SSID));
    S_WIFI_PASS = Settings.getString(KEY_WIFI_PASS, DEFAULT_WIFI_PASS);
    S_MQTT_ADDR = Settings.getString(KEY_MQTT_ADDR, DEFAULT_MQTT_ADDR);
    S_MQTT_USER = Settings.getString(KEY_MQTT_USER, DEFAULT_MQTT_USER);
    S_MQTT_PASS = Settings.getString(KEY_MQTT_PASS, DEFAULT_MQTT_PASS);
    S_MQTT_HA_DISCOVERY_PREFIX = Settings.getString(KEY_MQTT_HA_DISCOVERY_PREFIX, DEFAULT_MQTT_HA_DISCOVERY_PREFIX);
    S_MDNS_HOSTNAME = Settings.getString(KEY_MDNS_HOSTNAME, DEFAULT_MDNS_HOSTNAME);
    Settings.end();
}

void SettingsManager_::save()
{
    Settings.begin(SETTINGS_NS, RW_MODE);
    Serial.println("Settings save SSID: "+String(S_WIFI_SSID));
    Settings.putString(KEY_WIFI_SSID, S_WIFI_SSID);
    Settings.putString(KEY_WIFI_PASS, S_WIFI_PASS);
    Settings.putString(KEY_MQTT_ADDR, S_MQTT_ADDR);
    Settings.putString(KEY_MQTT_USER, S_MQTT_USER);
    Settings.putString(KEY_MQTT_PASS, S_MQTT_PASS);
    Settings.putString(KEY_MQTT_HA_DISCOVERY_PREFIX, S_MQTT_HA_DISCOVERY_PREFIX);
    Settings.putString(KEY_MDNS_HOSTNAME, S_MDNS_HOSTNAME);

    Settings.end();
}

void SettingsManager_::reset()
{
    Settings.begin(SETTINGS_NS, RW_MODE);
    Settings.clear();
    Settings.end();
}

void SettingsManager_::printToSerial()
{
    Serial.println("-----------Current Settings:----------");
    Serial.println("S_WIFI_SSID: "+ S_WIFI_SSID);
    Serial.println("S_WIFI_PASS: "+ S_WIFI_PASS);
    Serial.println("S_MQTT_ADDR: "+ S_MQTT_ADDR);
    Serial.print("S_MQTT_USER: ");
    Serial.println(S_MQTT_USER);
    Serial.print("S_MQTT_PASS: ");
    Serial.println(S_MQTT_PASS);
    Serial.println("S_MQTT_HA_DISCOVERY_PREFIX: "+ S_MQTT_HA_DISCOVERY_PREFIX);
    Serial.println("S_MDNS_HOSTNAME: "+ S_MDNS_HOSTNAME);
    Serial.println("--------------------------------------");

       
}

String S_WIFI_SSID;
String S_WIFI_PASS;
String S_MQTT_ADDR;
String S_MQTT_USER;
String S_MQTT_PASS;
String S_MQTT_HA_DISCOVERY_PREFIX;
String S_MDNS_HOSTNAME;
