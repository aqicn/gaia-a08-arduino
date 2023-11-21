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

SensirionI2CScd4x scd4x;

void scd4xSensorWorker();
Task scd4xTask(1000, TASK_FOREVER, &scd4xSensorWorker);
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

void scd4xSensorInit(Scheduler &runner)
{
    // Since we share the I2C interface with the met sensor,
    // make sure this is called after the met sensor init code.
    scd4x.begin(Wire);

    uint16_t error;
    char errorMessage[256];
    error = scd4x.stopPeriodicMeasurement();
    if (error)
    {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error)
    {
        Serial.print("Error trying to execute getSerialNumber(): ");
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
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
    runner.addTask(scd4xTask);
    scd4xTask.enable();
}

void scd4xSensorWorker()
{
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
        Serial.print("Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return;
    }
    if (!isDataReady)
    {
        return;
    }
    error = scd4x.readMeasurement(co2_ppm, temperature, humidity);
    if (error)
    {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else if (co2_ppm == 0)
    {
        Serial.println("Invalid sample detected, skipping.");
    }
    else
    {
        Serial.print("Co2:");
        Serial.print(co2_ppm);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);

        co2.add(co2_ppm);
    }
}
