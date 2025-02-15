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

/*
CONF_USE_WIFI_MANAGER: If defined, the device will use the WiFi manager
to configure the connection to the WiFi access point.
*/
// #define CONF_USE_WIFI_MANAGER

/*
CONF_USE_WEB_SERVER: If defined, the device will expose an HTTP
server from which one can get the real-time data.
*/
// #define CONF_USE_WEB_SERVER

// Get your own token from  https://aqicn.org/data-platform/token/
#define TOKEN "f092d2fe515e64a6e6f0add6dbbb364ff61903d0"

#define LATITUDE 51.8715557
#define LONGITUDE 5.2784883

#ifndef CONF_USE_WIFI_MANAGER
// Only needed if the WiFi manager is not used
#define WIFI_SSID "Lisro_IOT"
#define WIFI_PASS "Lisro@Home"
#define MQTT 0 // Comment or set to 0 to disable MQTT support in code

#define NUM_PMS_SENSORS 2
#endif
