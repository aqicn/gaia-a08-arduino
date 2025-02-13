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

#include <AHT20.h>
#include <Wire.h>
#include "sensors.hpp"
#include "main.hpp"

Accumulator<float> temperature, humidity;

AHT20 aht20;
void metSensorWorker();
Task metTask(5000, TASK_FOREVER, &metSensorWorker);

void metSensorInit(Scheduler &runner)
{
    Wire.begin(GPIO_SDA, GPIO_SCL);
    if (aht20.begin() == false)
    {
        Serial.println("AHT20 not detected. Please check wiring. Freezing.");
    }
    runner.addTask(metTask);
    metTask.enable();
}

void metSensorWorker()
{
    float t = aht20.getTemperature();
    float h = aht20.getHumidity();
    char s[48];
    snprintf(
        s,
        sizeof(s),
        "AHT20: tmp: %2.2f, hmd: %3.2f %% RH",
        t,
        h);
    Serial.println(s);

    temperature.add(t);
    humidity.add(h);
}
