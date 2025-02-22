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

#include "sensors.hpp"
#include "main.hpp"

#ifdef CONF_MQTT

#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"

esp_mqtt_client_handle_t client;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        Serial.println("Connected to MQTT Broker!");
        break;
    case MQTT_EVENT_DISCONNECTED:
        Serial.println("Disconnected from MQTT Broker.");
        break;
    case MQTT_EVENT_PUBLISHED:
        Serial.println("Data published to MQTT Broker");
        break;
    default:
        break;
    }
}

void mqttWorker(void *params)
{

    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS); // 10 seconds
        // Check WiFi connection status
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Error in WiFi connection");
            return;
        }

        JsonDocument doc;
        if (!getMinimalSensorData(doc))
        {
            return;
        }
        size_t json_len = measureJson(doc);
        static unsigned char json_body[256]; // expected json len is 225 bytes
        serializeJson(doc, json_body, sizeof(json_body));

        // Serial.printf("Posting: %s with len %d \n", json_body, json_len);

        if (esp_mqtt_client_publish(client, "GAIA/data", (char *)json_body, json_len, 1, 0) == -1)
        {
            Serial.println("Failed to publish data to MQTT Broker");
        }
    }
}

void mqttInit()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
        .port = MQTT_PORT,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
    };

    if (strlen(mqtt_cfg.uri) == 0)
    {
        Serial.println("Can not start the MQTT client: MQTT_BROKER_URI is not defined");
        return;
    }

    esp_err_t err;
    client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == nullptr)
    {
        Serial.println("Failed to create MQTT client");
        return;
    }

    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    err = esp_mqtt_client_start(client);
    if (err != ESP_OK)
    {
        Serial.println("Failed to start the MQTT client");
        return;
    }

    xTaskCreate(
        mqttWorker,   // Function that should be called
        "mqttWorker", // Name of the task (for debugging)
        2048,         // Stack size (bytes)
        NULL,         // Parameter to pass
        3,            // Task priority - medium
        NULL          // Task handle
    );
}
#endif
