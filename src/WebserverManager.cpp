#include "WebserverManager.h"
#include "ESPAsyncWebServer.h"
#include "SettingsManager.h"

AsyncWebServer server(80);
const char *PARAM_INPUT_WLAN_SSID = "inputWlanSsid";
const char *PARAM_INPUT_WLAN_PASS = "inputWlanPass";
const char *PARAM_INPUT_MQTT_ADDR = "inputMqttAddr";
const char *PARAM_INPUT_MQTT_USER = "inputMqttUser";
const char *PARAM_INPUT_MQTT_PASS = "inputMqttPass";
const char *PARAM_INPUT_MQTT_DATA_PREFIX = "inputMqttDataPrefix";
const char *PARAM_INPUT_MQTT_HA_PREFIX = "inputMqttHaAutoDiscoveryPrefix";




String getSSIDHtmlListItems()
{
    String temp = "";
    int16_t numNetworks = WifiManager.getNumNetworks();
    if (numNetworks > 0)
    {
        for (int16_t i = 0; i < numNetworks; i++)
        {
            temp += "<li>" + WifiManager.getNetworkInfoString(i) + "</li>";
        }
    }
    else
    {
        temp = "<li>No Networks available</li>";
    }

    return temp;
}

String processor(const String &var)
{
    if (var == "TMPL_SSID_LIST")
    {
        String temp = getSSIDHtmlListItems();
        return (temp);
    }
    else if (var == "TMPL_SSID")
    {
        return String(S_WIFI_SSID);
    }
    else if (var == "TMPL_MQTT_ADDR")
    {
        return String(S_MQTT_ADDR);
    }
    else if (var == "TMPL_MQTT_USER")
    {
        return String(S_MQTT_USER);
    }
    else if (var == "TMPL_MQTT_DATA_PREFIX")
    {
        return String(S_MQTT_DATA_PREFIX);
    }
    else if (var == "TMPL_MQTT_HA_PREFIX")
    {
        return String(S_MQTT_HA_DISCOVERY_PREFIX);
    }

 
    return String();
}

WebserverManager_ &WebserverManager_::getInstance()
{
    static WebserverManager_ instance;
    return instance;
}
void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        // request->addInterestingHeader("ANY");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
        response->print("<p>This is out captive portal front page.</p>");
        response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
        response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
        response->print("</body></html>");
        request->send(response);
    }
};

void WebserverManager_::setup()
{

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                Serial.println("HTTP_GET");
                request->send_P(200, "text/html", index_html, processor); 
                Serial.println("HTTP_GET DONE"); });
    server.onNotFound(notFound);
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              { ESP.restart(); });
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        SettingsManager.reset();
        ESP.restart(); });
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  if (request->hasParam(PARAM_INPUT_WLAN_SSID))
                  {
                      S_WIFI_SSID = request->getParam(PARAM_INPUT_WLAN_SSID)->value();
                      Serial.println(S_WIFI_SSID);
                  }
                  if (request->hasParam(PARAM_INPUT_WLAN_PASS))
                  {
                    String res= request->getParam(PARAM_INPUT_WLAN_PASS)->value();
                    if (res != "")
                    {
                        S_WIFI_PASS = request->getParam(PARAM_INPUT_WLAN_PASS)->value();
                      Serial.println(S_WIFI_PASS);
                    }
                    else
                    {
                        Serial.println("keep old wlan password");
                    }
  
                  }
                  if (request->hasParam(PARAM_INPUT_MQTT_ADDR))
                  {
                      S_MQTT_ADDR = request->getParam(PARAM_INPUT_MQTT_ADDR)->value();
                      Serial.println(S_MQTT_ADDR);
                  }
                  if (request->hasParam(PARAM_INPUT_MQTT_USER))
                  {
                      S_MQTT_USER = request->getParam(PARAM_INPUT_MQTT_USER)->value();
                      Serial.println(S_MQTT_USER);
                  }
                  if (request->hasParam(PARAM_INPUT_MQTT_PASS))
                  {
                    String res= request->getParam(PARAM_INPUT_MQTT_PASS)->value();
                    if (res != "")
                    {
                        S_MQTT_PASS = request->getParam(PARAM_INPUT_MQTT_PASS)->value();
                        Serial.println(S_MQTT_PASS);
                    }
                    else
                    {
                        Serial.println("keep old mqtt password");
                    }
                      
                  }
                  if (request->hasParam(PARAM_INPUT_MQTT_DATA_PREFIX))
                  {
                    S_MQTT_DATA_PREFIX= request->getParam(PARAM_INPUT_MQTT_DATA_PREFIX)->value();

                  }
                  if (request->hasParam(PARAM_INPUT_MQTT_HA_PREFIX))
                  {
                    S_MQTT_HA_DISCOVERY_PREFIX= request->getParam(PARAM_INPUT_MQTT_HA_PREFIX)->value();

                  }
                  
                  SettingsManager.save();
                  request->send(200, "text/html", "Settings saved, device will reboot");
                  ESP.restart(); });
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.begin();
}

WebserverManager_ &WebserverManager = WebserverManager.getInstance();
