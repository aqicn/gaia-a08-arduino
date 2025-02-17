/**
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.hpp"
#include <WiFi.h>
#include "network.hpp"
#include "sensors.hpp"
#include <ESPAsyncWebServer.h>

#ifdef CONF_USE_WEB_SERVER

AsyncWebServer server(80);

void webServerRealtimeHandler(AsyncWebServerRequest *request)
{
    JsonDocument doc;
    if (!pm25.hasData())
    {
        request->send(200, "application/json", "{\"status\":\"error\",\"reason\":\"no PM2.5 data\"}");
        return;
    }

    if (!getMinimalSensorData(doc))
    {
        request->send(200, "application/json", "{\"status\":\"error\"}");
        return;
    }

    static char json_body[512];
    serializeJson(doc, json_body, sizeof(json_body));
    request->send(200, "application/json", json_body);
}

void webServerInit()
{

    server.on("/realtime", HTTP_GET, webServerRealtimeHandler);
    server.begin();
}

#endif
