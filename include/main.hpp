#ifndef _GAIA_MAIN
#define _GAIA_MAIN

#define GPIO_RGB_LED 1
#define GPIO_5V_PWR_EN 2

#define GPIO_PMS1_RX 4
#define GPIO_PMS1_EN 5
#define GPIO_PMS2_EN 6
#define GPIO_PMS2_RX 7

#define GPIO_SDA 8
#define GPIO_SCL 9

#define GPIO_GREEN_LED 10

#define RGB_LED_COUNT 1

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.hpp"

extern char stationID[32];
extern char mac[32];

#endif // _GAIA_MAIN
