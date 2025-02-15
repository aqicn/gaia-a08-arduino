
#include <WiFi.h>
#include <HTTPClient.h>
#include "main.hpp"

#ifdef CONF_USE_WIFI_MANAGER
#include <WiFiManager.h>
WiFiManager wifiManager;
#endif

void wifiInit()
{
#ifdef CONF_USE_WIFI_MANAGER
    wifiManager.autoConnect("GAIA-A08");

#else
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        // Check for the connection
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.println("Trying to connecting to WiFi..");
    }
#endif

    Serial.print("Connected to the WiFi network with IP address: ");

    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}
