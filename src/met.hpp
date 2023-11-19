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

AHT20 aht20;
void metSensorWorker();
Task metTask(5000, TASK_FOREVER, &metSensorWorker);

Accumulator<float> temperature, humidity;

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

    // Print the results
    Serial.print("Temperature: ");
    Serial.print(t, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(h, 2);
    Serial.print("% RH");
    Serial.println();

    temperature.add(t);
    humidity.add(h);
}
