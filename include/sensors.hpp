#ifndef _GAIA_SENSORS
#define _GAIA_SENSORS

#include "accumulator.hpp"

// Sensor init functions
extern void scd4xSensorInit();
extern void pmsSensorInit();
extern void metSensorInit();

// Sensor value accumulators
extern Accumulator<int> co2;
extern Accumulator<float> temperature, humidity;
extern Accumulator<int> pm1, pm25, pm10;

#endif // _GAIA_SENSORS
