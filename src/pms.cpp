#include "PMS.h"
#include "sensors.hpp"
#include "main.hpp"

PMS pms(Serial1);
void pmsSensorWorker();
Task pmsTask(1000, TASK_FOREVER, &pmsSensorWorker);

void pmsSensorInit(Scheduler &runner)
{
    pinMode(GPIO_5V_PWR_EN, OUTPUT);
    digitalWrite(GPIO_5V_PWR_EN, HIGH);

    Serial1.begin(9600, SERIAL_8N1, GPIO_PMS2_RX, -1);
    runner.addTask(pmsTask);
    pmsTask.enable();
}

void pmsSensorWorker()
{
    static PMS::DATA data;
    if (pms.readUntil(data, 100))
    {
        char s[64];
        snprintf(
            s,
            sizeof(s),
            "PMS:   PM1.0: %5d, PM 2.5: %5d, PM10: %5d",
            data.PM_AE_UG_1_0,
            data.PM_AE_UG_2_5,
            data.PM_AE_UG_10_0);
        Serial.println(s);

        pm1.add(data.PM_AE_UG_1_0);
        pm25.add(data.PM_AE_UG_2_5);
        pm10.add(data.PM_AE_UG_10_0);
    }
}
