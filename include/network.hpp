
#ifndef _GAIA_NETWORK
#define _GAIA_NETWORK

#include <ArduinoJson.h>

void wifiInit();
void webServerInit();

#ifdef CONF_MQTT
void mqttInit();
#endif

#endif // _GAIA_NETWORK
