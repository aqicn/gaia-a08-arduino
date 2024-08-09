
#ifdef CONF_USE_WEB_SERVER
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void webServerRealtimeHandler(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    if (!pm25.hasData())
    {
        request->send(200, "application/json", "{\"status\":\"error\",\"reason\":\"no PM2.5 data\"}");
        return;
    }

    if (!uploaderGetCurrentStatus(doc))
    {
        request->send(200, "application/json", "{\"status\":\"error\"}");
        return;
    }

    static char json_body[1024 * 8];
    serializeJson(doc, json_body);
    request->send(200, "application/json", json_body);
}

void webServerInit(Scheduler &runner)
{

    server.on("/realtime", HTTP_GET, webServerRealtimeHandler);

    server.begin();
}

#else
void webServerInit(Scheduler &runner)
{
}
#endif