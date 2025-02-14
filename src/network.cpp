#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

#include "network.hpp"
#include "main.hpp"
#include "utils.hpp"

#define SNTP_GET_SERVERS_FROM_DHCP 1
#include <esp_sntp.h>

struct tm timeinfo;
bool LocalTimeSet = false;

#include "esp_log.h"

struct mg_mgr mgr; // Mongoose event manager. Holds all connections

String APhostname = "GAIA-08-" + String(MacId() & 0xffff, 10);

#if MQTT
// MQTT connection info
String MQTTuser;
String MQTTpassword;
String MQTTprefix;
String MQTTHost = "";
uint16_t MQTTPort;
mg_timer *MQTTtimer;
uint8_t lastMqttUpdate = 0;
#endif

uint8_t WIFImode = WIFI_MODE; // WiFi Mode (0:Disabled / 1:Enabled / 2:Start Portal)

#if MQTT
MQTTclient_t MQTTclient;

void SetupMQTTClient()
{
    // Set up subscriptions
    MQTTclient.subscribe(MQTTprefix + "/Set/#", 1); // TODO: What to listen to?
    MQTTclient.publish(MQTTprefix + "/connected", "online", true, 0);
}

void MQTTclient_t::publish(const String &topic, const String &payload, bool retained, int qos)
{
    if (connected)
    {
        esp_mqtt_client_enqueue(client, topic.c_str(), payload.c_str(), payload.length(), qos, retained, 1);
        // esp_mqtt_client_publish(client, topic.c_str(), payload.c_str(), payload.length(), qos, retained);
    }
}
/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, esp_mqtt_event_t *event)
{
    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        log_i("MQTT_EVENT_CONNECTED");
        MQTTclient.connected = true;
        SetupMQTTClient();
        break;
    case MQTT_EVENT_DISCONNECTED:
        MQTTclient.connected = false;
        log_i("MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        log_i("MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        log_i("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        log_i("MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        log_i("MQTT_EVENT_DATA");
        break;
    case MQTT_EVENT_ERROR:
        log_i("MQTT_EVENT_ERROR");
        break;
    default:
        log_i("Other event id:%d", event->event_id);
        break;
    }
}

#endif

// turns out getLocalTime only checks if the current year > 2016, and if so, decides NTP must have synced;
// this callback function actually checks if we are synced!
void timeSyncCallback(struct timeval *tv)
{
    LocalTimeSet = true;
    log_i("Synced clock to NTP server!");
}

void onWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_START:
        log_i("WiFi started");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        log_i("WiFi connected to %s Local IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        // load dhcp dns ip4 address into mongoose
        static char dns4url[] = "udp://123.123.123.123:53";
        sprintf(dns4url, "udp://%s:53", WiFi.dnsIP().toString().c_str());
        mgr.dns4.url = dns4url;

        // TODO: MDNS?
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        log_i("Connected or reconnected to WiFi\n");
#if MQTT
        if (MQTTHost != "")
        {
            esp_mqtt_client_start(MQTTclient.client);
        }
        // TODO: Setup webserver
#endif
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        log_i("WiFi disconnected from %s", WiFi.SSID().c_str());

        if (WIFImode == 1)
        {
            WiFi.reconnect(); // recommended reconnection strategy by ESP-IDF manual
        }
        break;
    case ARDUINO_EVENT_SC_GOT_SSID_PSWD:
        log_i("WiFi stopped");
        break;
    default:
        break;
    }
}

void handleWIFImode(void *s)
{
    if (WIFImode == 2 && WiFi.getMode() != WIFI_AP_STA)
    {
        // TODO, start AP
    }

    if (WIFImode == 1 && WiFi.getMode() == WIFI_OFF)
    {
        log_i("Starting WiFi..\n");
        WiFi.mode(WIFI_STA);
        WiFi.begin();
    }

    if (WIFImode == 0 && WiFi.getMode() != WIFI_OFF)
    {
        log_i("Stopping WiFi..\n");
        WiFi.disconnect(true);
    }
}

void WiFiSetup()
{
    WiFi.setHostname(APhostname.c_str()); // TODO: Create small website to configure wifi
    mg_mgr_init(&mgr);                    // Initialise event manager
    WiFi.setAutoReconnect(true);
    WiFi.onEvent(onWifiEvent);
    sntp_servermode_dhcp(1);                      // try to get the ntp server from dhcp
    sntp_setservername(1, "europe.pool.ntp.org"); // fallback server
    sntp_set_time_sync_notification_cb(timeSyncCallback);
    sntp_init();
    handleWIFImode();
}

// called by loop() in the main program
void network_loop()
{
    static unsigned long lastCheck_net = 0;
    static int seconds = 0;
    if (millis() - lastCheck_net >= 1000)
    {
        lastCheck_net = millis();
        // this block is for non-time critical stuff that needs to run approx 1 / second
        getLocalTime(&timeinfo, 1000U);
        if (!LocalTimeSet && WIFImode == 1)
        {
            log_w("Time not synced with NTP yet.");
        }
        // this block is for non-time critical stuff that needs to run approx 1 / 10 seconds
#if MQTT
        if (seconds++ >= 9)
        {
            seconds = 0;
            MQTTclient.publish(MQTTprefix + "/ESPUptime", esp_timer_get_time() / 1000000, false, 0);
            MQTTclient.publish(MQTTprefix + "/WiFiRSSI", String(WiFi.RSSI()), false, 0);
        }
#endif
    }
    mg_mgr_poll(&mgr, 100); // TODO increase this parameter to up to 1000 to make loop() less greedy
}
