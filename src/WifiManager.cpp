#include <WifiManager.h>
#define AP_SSID_BASE "Elekstube-R_"
#define AP_PASS_BASE "password"
#define WL_MAC_LENGTH 6

WifiManager_ &WifiManager_::getInstance()
{
    static WifiManager_ instance;
    return instance;
}

WifiManager_ &WifiManager = WifiManager.getInstance();
DNSServer dnsServer;
int status = WL_IDLE_STATUS;
bool setupLoopBusy = true;
int16_t numNetworks = 0;
String ssid = "";
String password = "";
String macAddr = "";
uint8_t macAddrByte[WL_MAC_LENGTH];
int ApPasscode = 0; 

void WifiManager_::begin()
{
    macAddr = WiFi.macAddress();
    WiFi.macAddress(macAddrByte);
    // with setting the seed using the mac address, the passcode stays the same for the same device
    randomSeed(macAddrByte[WL_MAC_LENGTH - 2] << 16 + macAddrByte[WL_MAC_LENGTH - 1]);
    ApPasscode = random(100000, 999999);
    // try to connect to wifi from setting manager
    Serial.println("Connecting to SSID: "+String(S_WIFI_SSID));
    WiFi.begin(S_WIFI_SSID, S_WIFI_PASS);
    currentWifiMode = WIFIMANAGER_MODE_BUSY;
    while (setupLoopBusy)
    {
        status = WiFi.status();
        switch (status)
        {
        case WL_IDLE_STATUS:
            break;
        case WL_NO_SSID_AVAIL:
        case WL_CONNECT_FAILED:
        case WL_CONNECTION_LOST:
            targetWifiMode = WIFIMANAGER_MODE_AP;
            WiFi.setAutoReconnect(false);
            WiFi.disconnect();
            setupLoopBusy = false;
            break;
        case WL_CONNECTED:
            targetWifiMode = WIFIMANAGER_MODE_CONNECTED;
            WiFi.setAutoReconnect(true);

            setupLoopBusy = false;
            break;
        default:
            break;
        }
        // wait 10 seconds for connection:
        delay(500);
    }

    String macSuffix = macAddr;
    macSuffix.replace(":", "");
    macSuffix = macSuffix.substring(macSuffix.length() - 4);

    switch (targetWifiMode)
    {
    case WIFIMANAGER_MODE_CONNECTED:
        currentWifiMode = WIFIMANAGER_MODE_CONNECTED;
        
        break;
    case WIFIMANAGER_MODE_AP:
        // scan networks

        numNetworks = WiFi.scanNetworks();
        ssid = AP_SSID_BASE + macSuffix;
        password = AP_PASS_BASE + String(ApPasscode);
        Serial.println("AP SSID: " + ssid);
        Serial.println("AP PW: "+password);
        WiFi.softAP(ssid, password);
        dnsServer.start(53, "*", WiFi.softAPIP());
        currentWifiMode = WIFIMANAGER_MODE_AP;
        break;
    default:
        break;
    }
}

WifiMode WifiManager_::getMode()
{
    return currentWifiMode;
}

int WifiManager_::getApPasscode()
{
    return ApPasscode;
}

int16_t WifiManager_::getNumNetworks()
{
    return numNetworks;
}

String getEncryptionTypeString(wifi_auth_mode_t mode)
{
    switch (mode)
    {
    case WIFI_AUTH_OPEN:
        return "none";
        break;
    case WIFI_AUTH_WEP:
        return "WEP";
        break;
    case WIFI_AUTH_WPA_PSK:
        return "WPA";
        break;
    case WIFI_AUTH_WPA2_PSK:
        return "WPA2_PSK";
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WPA_WPA2_PSK";
        break;
    case WIFI_AUTH_ENTERPRISE:
        return "WiFi EAP security";
        break;
    case WIFI_AUTH_WPA3_PSK:
        return "WPA3_PSK";
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WPA2_WPA3_PSK";
        break;
    case WIFI_AUTH_WAPI_PSK:
        return "WAPI_PSK";
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        return "WPA3_ENT_SUITE_B_192_BIT";
        break;
    default:
        return "undefined";
        break;
    }
}

String WifiManager_::getNetworkInfoString(uint8_t num)
{

    if (num < numNetworks)
    {
        return WiFi.SSID(num) +" : "+ getEncryptionTypeString(WiFi.encryptionType(num));
    }
    else
    {
        return "n/a";
    }
}