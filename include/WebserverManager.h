#ifndef WebserverManager_h
#define WebserverManager_h
#include "WifiManager.h"

class WebserverManager_
{
private:
    WebserverManager_() = default;
    

public:
    static WebserverManager_ &getInstance();
    void setup();
};

extern WebserverManager_ &WebserverManager;

// HTML web page basics
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>Elekstube-R ESP32</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        .mycontainer {
            display: flex;
        }



        .console div {
            float: bottom;
            clear: none;
        }
    </style>
    <script>
        function requestReboot() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/reboot", true);
            xhttp.send();
        }
    </script>
    <script>
        function requestFactoryReset() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/reset", true);
            xhttp.send();
        }
    </script>
</head>

<body>
    <div class="main">
        <div style="background-color:#FFF4A3;">
            <p>Settings</p>
            <form action="/get">
                <table>
                    <tr>
                        <td>SSID</td>
                        <td><input type="text" value="%TMPL_SSID%" name="inputWlanSsid"></td>
                    </tr>
                    <tr>
                        <td>Password</td>
                        <td><input type="password" value="" name="inputWlanPass"></td>
                    </tr>
                    <tr>
                        <td>MDNS Hostname</td>
                        <td><input type="text" value="%TMPL_MDNS_HOSTNAME%" name="inputMdnsHostname"></td>
                    </tr>
                    <tr>
                        <td>MQTT Servername</td>
                        <td><input type="text" value="%TMPL_MQTT_ADDR%" name="inputMqttAddr"></td>
                    </tr>
                    <tr>
                        <td>MQTT Username</td>
                        <td><input type="text" value="%TMPL_MQTT_USER%" name="inputMqttUser"></td>
                    </tr>
                    <tr>
                        <td>MQTT Password</td>
                        <td><input type="password" value="" name="inputMqttPass"></td>
                    </tr>
                    <tr>
                        <td>MQTT data prefix</td>
                        <td><input type="text" value="%TMPL_MQTT_DATA_PREFIX%" name="inputMqttDataPrefix"></td>
                    </tr>
                    <tr>
                        <td>MQTT HA autodiscovery prefix (usually "homeassistant")</td>
                        <td><input type="value" value="%TMPL_MQTT_HA_PREFIX%" name="inputMqttHaAutoDiscoveryPrefix"></td>
                    </tr>
                </table>
               
                <input type="submit" value="Save Settings and Reboot"><br>
            </form>

            <input type="submit" onclick="requestReboot();" value="Reboot"><br>
            <input type="submit" onclick="requestFactoryReset();" value="Reset Preferences">
        </div>
        <div style="background-color:#FFC0C7;">
            <p>List of available SSIDs</p>
            <ul>
                %TMPL_SSID_LIST%
            </ul>
        </div>
    </div>
    <div class="console" style="background-color:#D9EEE1;">
        <p>Console output</p>
    </div>
</body>

</html>
)rawliteral";



extern String S_WIFI_SSID;
extern String S_WIFI_PASS;
extern String S_MQTT_ADDR;
extern String S_MQTT_USER;
extern String S_MQTT_PASS;
extern String S_MQTT_HA_DISCOVERY_PREFIX;
extern String S_MDNS_HOSTNAME;
#endif