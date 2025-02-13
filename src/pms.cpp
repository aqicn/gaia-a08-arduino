#include "PMS.h"
#include "sensors.hpp"
#include "main.hpp"
#include "config.hpp"

Accumulator<int> pm1, pm25, pm10;

void pms1SensorWorker();
PMS pms1(Serial0);
Task pms1Task(1000, TASK_FOREVER, &pms1SensorWorker);
#if NUM_PMS_SENSORS == 2
PMS pms2(Serial1);
void pms2SensorWorker();
Task pms2Task(1000, TASK_FOREVER, &pms2SensorWorker);
#endif

void pmsSensorInit(Scheduler &runner)
{
    pinMode(GPIO_5V_PWR_EN, OUTPUT);
    digitalWrite(GPIO_5V_PWR_EN, HIGH);

    Serial0.begin(9600, SERIAL_8N1, GPIO_PMS1_RX, -1);
    runner.addTask(pms1Task);
    pms1Task.enable();

#if NUM_PMS_SENSORS == 2
    Serial1.begin(9600, SERIAL_8N1, GPIO_PMS2_RX, -1);
    runner.addTask(pms2Task);
    pms2Task.enable();
#endif
}

void processSensorData(int pms_num, PMS::DATA data)
{

    char s[64];
    snprintf(
        s,
        sizeof(s),
        "PMS%1d:  PM1.0: %5d, PM 2.5: %5d, PM10: %5d",
        pms_num,
        data.PM_AE_UG_1_0,
        data.PM_AE_UG_2_5,
        data.PM_AE_UG_10_0);

    Serial.println(s);

    pm1.add(data.PM_AE_UG_1_0);
    pm25.add(data.PM_AE_UG_2_5);
    pm10.add(data.PM_AE_UG_10_0);
}

void pms1SensorWorker()
{
    static PMS::DATA pms1_data;
    if (pms1.readUntil(pms1_data, 100))
    {
        processSensorData(1, pms1_data);
    }
}

#if NUM_PMS_SENSORS == 2
void pms2SensorWorker()
{
    static PMS::DATA pms2_data;
    if (pms2.readUntil(pms2_data, 100))
    {
        processSensorData(2, pms2_data);
    }
}
#endif
