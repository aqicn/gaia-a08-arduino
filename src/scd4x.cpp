/**
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <SensirionI2CScd4x.h>
#include "main.hpp"
#include "sensors.hpp"

SensirionI2CScd4x scd4x;

void scd4xSensorWorker(void *parameter);
int scd4xErrorCount = 0;

Accumulator<int> co2;

void printUint16Hex(uint16_t value)
{
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2)
{
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void scd4xSensorWorker(void *parameter)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        uint16_t error;
        char errorMessage[256];

        delay(100);

        // Read Measurement
        uint16_t co2_ppm = 0;
        float temperature = 0.0f;
        float humidity = 0.0f;
        bool isDataReady = false;
        error = scd4x.getDataReadyFlag(isDataReady);
        if (error)
        {
            Serial.print("SCD4x: Error trying to execute getDataReadyFlag(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
            continue;
        }
        if (!isDataReady)
        {
            continue;
        }
        error = scd4x.readMeasurement(co2_ppm, temperature, humidity);
        if (error)
        {
            Serial.print("SCD4x: Error trying to execute readMeasurement(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        }
        else if (co2_ppm == 0)
        {
            Serial.println("SCD4x: Invalid sample detected, skipping.");
        }
        else
        {
#if SERIAL_VALUES

            char s[80];
            snprintf(
                s,
                sizeof(s),
                "SCD4x: tmp: %2.2f, hmd: %2.2f, Co2: %5d",
                temperature,
                humidity,
                co2_ppm);
            Serial.println(s);
#endif
            co2.add(co2_ppm);
        }

    } // while(1)
}

void scd4xSensorInit()
{
    InitializeI2C();
    scd4x.begin(Wire);

    uint16_t error;
    char errorMessage[256];
    error = scd4x.stopPeriodicMeasurement();
    if (error)
    {
        if (++scd4xErrorCount <= 3)
        {
            Serial.print("SCD4x: Error trying to execute stopPeriodicMeasurement(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
        }
        return;
    }
    scd4xErrorCount = 0;

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error)
    {
        Serial.print("SCD4x: Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else
    {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error)
    {
        Serial.print("SCD4x: Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("SCD4x: Waiting for first measurement... (5 sec)");

    xTaskCreate(
        scd4xSensorWorker,   // Function that should be called
        "scd4xSensorWorker", // Name of the task (for debugging)
        4096,                // Stack size (bytes)
        NULL,                // Parameter to pass
        3,                   // Task priority - medium
        NULL                 // Task handle
    );
}