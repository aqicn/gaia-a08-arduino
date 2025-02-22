#include "PMS.h"
#include "sensors.hpp"
#include "main.hpp"
#include "config.hpp"
#include "indicator.hpp"

/*

When using two PMS sensors, the sensor are activate alternately:
Each sensor run for two minutes, and then the next sensor is activated.

Activating a sensor can be done by pulling up the PMS enable pin.
By defualt, the pin is pulled-up. So only the deactivation is needed,
and the deactivation is done by pulling down the pin.

When transitining from deactivated to active, the sensor fan will start to  spin.
It is a common practice to wait 30 seconds before using the sensor values.

*/

Accumulator<int> pm1, pm25, pm10;

void pmsSensorWorker(void *);
PMS pms1(Serial0);

#if NUM_PMS_SENSORS == 2
PMS pms2(Serial1);
#endif

void pmsSensorInit()
{
    pinMode(GPIO_5V_PWR_EN, OUTPUT);
    digitalWrite(GPIO_5V_PWR_EN, HIGH);

    Serial0.begin(9600, SERIAL_8N1, GPIO_PMS1_RX, -1);
#if NUM_PMS_SENSORS == 2
    Serial1.begin(9600, SERIAL_8N1, GPIO_PMS2_RX, -1);
#endif

    xTaskCreate(
        pmsSensorWorker,   // Function that should be called
        "pmsSensorWorker", // Name of the task (for debugging)
        2048,              // Stack size (bytes)
        NULL,              // Parameter to pass
        3,                 // Task priority - medium
        NULL               // Task handle
    );
}

void processSensorData(int pms_num, PMS::DATA &data)
{
    pm1.add(data.PM_AE_UG_1_0);
    pm25.add(data.PM_AE_UG_2_5);
    pm10.add(data.PM_AE_UG_10_0);
}

void dumpSensorData(int pms_num, PMS::DATA &data)
{
#ifdef DEBUG_SENSOR_VALUES
    char s[64];
    snprintf(
        s,
        sizeof(s),
        "PMS%1d:  PM1.0: %5d, PM 2.5: %5d, PM10: %5d",
        pms_num + 1,
        data.PM_AE_UG_1_0,
        data.PM_AE_UG_2_5,
        data.PM_AE_UG_10_0);

    Serial.println(s);
#endif
}

void pmsSensorWorker(void *parameters)
{
#if NUM_PMS_SENSORS == 2
    // duty_cyle: activity time for each PMS sensor - 2 minutes
    int duty_cycle = 120;
    // time before the samples are actively used - 30 seconds
    int wait_time = 30;
    // current PMS sensor
    int current_pms = 0;

    static PMS::DATA pms_data;

    int enable_pins[NUM_PMS_SENSORS] = {GPIO_PMS1_EN, GPIO_PMS2_EN};
    PMS *pms_readers[NUM_PMS_SENSORS] = {&pms1, &pms2};

    // Configure all sensors to sleep
    for (int i = 0; i < NUM_PMS_SENSORS; i++)
    {
        pinMode(enable_pins[i], OUTPUT);
        digitalWrite(enable_pins[i], LOW);
    }

    while (true)
    {

        digitalWrite(enable_pins[current_pms], HIGH);

        int start = millis() / 1000;
        PMS *pms = pms_readers[current_pms];
        while (millis() / 1000 < start + duty_cycle)
        {
            if (pms->readUntil(pms_data, 100))
            {
                dumpSensorData(current_pms, pms_data);
                indicatorReportPm25(pms_data.PM_AE_UG_2_5);
                if (millis() / 1000 > start + wait_time)
                {
                    processSensorData(current_pms, pms_data);
                }
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        digitalWrite(enable_pins[current_pms], LOW);
        current_pms = (current_pms + 1) % NUM_PMS_SENSORS;
        Serial.printf("Switching to PMS sensor %d\n", current_pms + 1);
    }
#else
    static PMS::DATA pms1_data;
    while (1)
    {
        if (pms1.readUntil(pms1_data, 100))
        {
            dumpSensorData(0, pms1_data);
            indicatorReportPm25(pms1_data.PM_AE_UG_2_5);
            processSensorData(0, pms1_data);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif
}
