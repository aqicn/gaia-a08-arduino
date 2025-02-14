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

#include "main.hpp"
#include "config.hpp"
#include "sensors.hpp"
#include "mongoose.h"
#include "network.hpp"
#include <WiFi.h>
#include "uploader.hpp"

void uploaderWorker(void *parameter);

static const char *s_url = "https://aqicn.org/sensor/upload/";
struct mg_str host = mg_url_host(s_url);
static const char *s_post_data = NULL;     // POST data
static const uint64_t s_timeout_ms = 1500; // Connect timeout in milliseconds

static void fn(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_OPEN)
    {
        // Connection created. Store connect expiration time in c->data
        *(uint64_t *)c->data = mg_millis() + s_timeout_ms;
    }
    else if (ev == MG_EV_POLL)
    {
        if (mg_millis() > *(uint64_t *)c->data &&
            (c->is_connecting || c->is_resolving))
        {
            mg_error(c, "Connect timeout");
        }
    }
    else if (ev == MG_EV_CONNECT)
    {
        // Connected to server. Extract host name from URL
        struct mg_str host = mg_url_host(s_url);

        if (mg_url_is_ssl(s_url))
        {
            struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ca.pem"),
                                       .name = mg_url_host(s_url)};
            mg_tls_init(c, &opts);
        }

        // Send request
        int content_length = s_post_data ? strlen(s_post_data) : 0;
        mg_printf(c,
                  "%s %s HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "Content-Type: application/json\r\n"
                  "Content-Length: %d\r\n"
                  "User-Agent: GAIA-uploader/1.1"
                  "\r\n",
                  s_post_data ? "POST" : "GET",
                  mg_url_uri(s_url),
                  (int)host.len,
                  host.buf,
                  content_length);
        mg_send(c, s_post_data, content_length);
    }
    else if (ev == MG_EV_HTTP_MSG)
    {
        // Response is received. Print it
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        printf("%.*s", (int)hm->message.len, hm->message.buf);
        c->is_draining = 1;         // Tell mongoose to close this connection
        *(bool *)c->fn_data = true; // Tell event loop to stop
    }
    else if (ev == MG_EV_ERROR)
    {
        log_e("Error: %s", (char *)ev_data);
        *(bool *)c->fn_data = true; // Error, tell event loop to stop
    }
}
void uploaderInit()
{
    // Create Task 100ms Timer
    xTaskCreate(
        uploaderWorker,   // Function that should be called
        "uploaderWorker", // Name of the task (for debugging)
        4608,             // Stack size (bytes)
        NULL,             // Parameter to pass
        3,                // Task priority - medium
        NULL              // Task handle
    );
}

bool uploaderGetCurrentStatus(JsonDocument &doc)
{
    if (!pm25.hasData())
    {
        return false;
    }
    uint64_t efuseMac = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    static char mac[32];
    snprintf(mac, 32, "%llx", efuseMac);

    static char stationID[32];
    uint16_t chip = (uint16_t)(efuseMac >> 32);
    snprintf(stationID, 32, "GAIA-A08-%x", chip);
    Serial.printf("device ID is '%s'\n", stationID);

    doc["station"]["id"] = stationID;
    doc["station"]["mac"] = mac;

    doc["station"]["location"]["latitude"] = LATITUDE;
    doc["station"]["location"]["longitude"] = LONGITUDE;

    doc["readings"][0]["specie"] = "pm25";
    doc["readings"][0]["value"] = pm25.avg();
    doc["readings"][0]["unit"] = "µg/m3";

    doc["readings"][1]["specie"] = "pm10";
    doc["readings"][1]["value"] = pm10.avg();
    doc["readings"][1]["unit"] = "µg/m3";

    doc["readings"][2]["specie"] = "pm1";
    doc["readings"][2]["value"] = pm1.avg();
    doc["readings"][2]["unit"] = "µg/m3";

    doc["readings"][3]["specie"] = "temperature";
    doc["readings"][3]["value"] = temperature.avg();
    doc["readings"][3]["unit"] = "C";

    doc["readings"][4]["specie"] = "humidity";
    doc["readings"][4]["value"] = humidity.avg();
    doc["readings"][4]["unit"] = "%";

    if (co2.hasData())
    {
        doc["readings"][4]["specie"] = "co2";
        doc["readings"][4]["value"] = co2.avg();
        doc["readings"][4]["unit"] = "ppm";
    }

    doc["token"] = TOKEN;

    return true;
}

void uploaderResetCurrentStatus()
{
    pm10.reset();
    pm25.reset();
    pm1.reset();
    temperature.reset();
    humidity.reset();
    co2.reset();
}

void uploaderWorker(void *parameter)
{
    // Check WiFi connection status
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Error in WiFi connection");
        return;
    }

    JsonDocument doc;
    if (!uploaderGetCurrentStatus(doc))
    {
        return;
    }
    uploaderResetCurrentStatus();

    Serial.print("Posting: ");
    serializeJson(doc, Serial);
    Serial.println("");

    static char json_body[1024 * 8];
    serializeJson(doc, json_body);
    s_post_data = json_body;

    bool done = false;                       // Event handler flips it to true
    mg_http_connect(&mgr, s_url, fn, &done); // Create client connection

    vTaskDelay(60 * (1000 / portTICK_PERIOD_MS)); // Upload data once every 60 seconds
}