#ifndef _GAIA_SENSORS
#define _GAIA_SENSORS

#include <TaskScheduler.h>
#include "accumulator.hpp"

// Sensor init functions
extern void scd4xSensorInit(Scheduler &runner);
extern void pmsSensorInit(Scheduler &runner);
extern void metSensorInit(Scheduler &runner);

// Sensor value accumulators
extern Accumulator<int> co2;
extern Accumulator<float> temperature, humidity;
extern Accumulator<int> pm1, pm25, pm10;

#endif // _GAIA_SENSORS
