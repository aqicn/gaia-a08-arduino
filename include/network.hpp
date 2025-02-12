
#ifndef _GAIA_NETWORK
#define _GAIA_NETWORK

#include "mongoose.h"
#include <ArduinoJson.h>

#ifndef MQTT
#define MQTT 1 // Uncomment or set to 0 to disable MQTT support in code
#endif         // MQTT

#if MQTT
#include "mqtt_client.h"

// MQTT connection info
extern String MQTTuser;
extern String MQTTpassword;
extern String MQTTprefix;
extern String MQTTHost;
extern uint16_t MQTTPort;
extern uint8_t lastMqttUpdate;

class MQTTclient_t
{

public:
    void connect(void);
    void disconnect(void) { esp_mqtt_client_stop(client); };
    void publish(const String &topic, const int32_t &payload, bool retained, int qos) { publish(topic, String(payload), retained, qos); };
    void publish(const String &topic, const String &payload, bool retained, int qos);
    void subscribe(const String &topic, int qos);
    bool connected;
    esp_mqtt_client_handle_t client;
};

extern MQTTclient_t MQTTclient;
extern void SetupMQTTClient();
extern void mqtt_receive_callback(const String topic, const String payload);

#endif // MQTT

// wrapper so hasParam and getParam still work
class webServerRequest
{
private:
    struct mg_http_message *hm_internal;
    String _value;
    char temp[64];

public:
    void setMessage(struct mg_http_message *hm);
    bool hasParam(const char *param);
    webServerRequest *getParam(const char *param); // Return pointer to self
    const String &value();                         // Return the string value
};

extern bool shouldReboot;
extern void network_loop(void);
extern String APhostname;
extern webServerRequest *request;
extern struct mg_mgr mgr;
extern uint8_t WIFImode;
extern void WiFiSetup(void);
extern void handleWIFImode(void *s = &Serial);

#endif // _GAIA_NETWORK
