#ifndef _GAIA_SENSORS
#define _GAIA_SENSORS

#include <TaskScheduler.h>
#include "accumulator.hpp"

// Sensor init functions
void scd4xSensorInit(Scheduler &runner);
void pmsSensorInit(Scheduler &runner);
void metSensorInit(Scheduler &runner);

// Sensor value accumulators
Accumulator<int> co2;
Accumulator<float> temperature, humidity;
Accumulator<int> pm1, pm25, pm10;

#endif // _GAIA_SENSORS
