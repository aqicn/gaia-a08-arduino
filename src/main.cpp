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

// -----------------------
#include "main.hpp"

#ifdef USBSerial
#define Serial USBSerial
#endif

// -----------------------
#include "config.hpp"
#include "accumulator.hpp"
#include "sensors.hpp"
#include "indicator.hpp"
#include "network.hpp"
#include "uploader.hpp"

// -----------------------
char stationID[32];
char mac[32];

void getStationId()
{
    uint64_t efuseMac = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    snprintf(mac, 32, "%llx", efuseMac);
    uint16_t chip = (uint16_t)(efuseMac >> 32);
    snprintf(stationID, 32, "GAIA-A08-%x", chip);
    Serial.printf("device ID is '%s'\n", stationID);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("starting...");

    getStationId();

    metSensorInit();
    pmsSensorInit();
    rgbLedInit();
    ledInit();
    scd4xSensorInit();
    uploaderInit();

    wifiInit();
#ifdef CONF_MQTT
    mqttInit();
#endif
#ifdef CONF_USE_WEB_SERVER
    webServerInit();
#endif
}

void loop()
{
    rgbLedLoop();
}
