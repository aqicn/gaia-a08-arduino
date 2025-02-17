#include "PMS.h"
#include "sensors.hpp"
#include "main.hpp"
#include "config.hpp"

Accumulator<int> pm1, pm25, pm10;

void pms1SensorWorker(void *);
PMS pms1(Serial0);
#if NUM_PMS_SENSORS == 2
PMS pms2(Serial1);
void pms2SensorWorker(void *);
#endif

void pmsSensorInit()
{
    pinMode(GPIO_5V_PWR_EN, OUTPUT);
    digitalWrite(GPIO_5V_PWR_EN, HIGH);

    Serial0.begin(9600, SERIAL_8N1, GPIO_PMS1_RX, -1);
    xTaskCreate(
        pms1SensorWorker,   // Function that should be called
        "pms1SensorWorker", // Name of the task (for debugging)
        2048,               // Stack size (bytes)
        NULL,               // Parameter to pass
        3,                  // Task priority - medium
        NULL                // Task handle
    );

#if NUM_PMS_SENSORS == 2
    Serial1.begin(9600, SERIAL_8N1, GPIO_PMS2_RX, -1);
    xTaskCreate(
        pms2SensorWorker,   // Function that should be called
        "pms2SensorWorker", // Name of the task (for debugging)
        2048,               // Stack size (bytes)
        NULL,               // Parameter to pass
        4,                  // Task priority - medium
        NULL                // Task handle
    );
#endif
}

void processSensorData(int pms_num, PMS::DATA data)
{
#ifdef DEBUG_SENSOR_VALUES
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
#endif

    pm1.add(data.PM_AE_UG_1_0);
    pm25.add(data.PM_AE_UG_2_5);
    pm10.add(data.PM_AE_UG_10_0);
}

void pms1SensorWorker(void *parameters)
{
    static PMS::DATA pms1_data;
    while (1)
    {
        if (pms1.readUntil(pms1_data, 100))
        {
            processSensorData(1, pms1_data);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

#if NUM_PMS_SENSORS == 2
void pms2SensorWorker(void *parameters)
{
    static PMS::DATA pms2_data;
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if (pms2.readUntil(pms2_data, 100))
        {
            processSensorData(2, pms2_data);
        }
    }
}
#endif
