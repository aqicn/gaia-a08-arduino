#ifndef _GAIA_SENSORS
#define _GAIA_SENSORS

#include "accumulator.hpp"
#include <ArduinoJson.h>

// Sensor init functions
extern void scd4xSensorInit();
extern void pmsSensorInit();
extern void metSensorInit();

void InitializeI2C();

// Sensor value accumulators
extern Accumulator<int> co2;
extern Accumulator<float> temperature, humidity;
extern Accumulator<int> pm1, pm25, pm10;

extern bool getSerialisedSensorData(JsonDocument &doc);
extern bool getMinimalSensorData(JsonDocument &doc);
extern bool i2c_initialized;

#endif // _GAIA_SENSORS
