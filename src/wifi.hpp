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

#include <WiFi.h>
#include <HTTPClient.h>

#ifdef CONF_USE_WIFI_MANAGER
#include <WiFiManager.h>
WiFiManager wifiManager;
#endif

void wifiInit(Scheduler &runner)
{
#ifdef CONF_USE_WIFI_MANAGER
    wifiManager.autoConnect("GAIA-A08");

#else
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        // Check for the connection
        delay(1000);
        Serial.println("Trying to connecting to WiFi..");
    }
#endif

    Serial.print("Connected to the WiFi network with IP address: ");

    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}
