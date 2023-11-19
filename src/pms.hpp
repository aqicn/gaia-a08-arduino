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

#include "PMS.h"

PMS pms(Serial1);
void pmsSensorWorker();
Task pmsTask(1000, TASK_FOREVER, &pmsSensorWorker);

Accumulator<int> pm25, pm10, pm1;

void pmsSensorInit(Scheduler &runner)
{
    pinMode(GPIO_5V_PWR_EN, OUTPUT);
    digitalWrite(GPIO_5V_PWR_EN, HIGH);

    Serial1.begin(9600, SERIAL_8N1, GPIO_PMS1_RX, -1);
    runner.addTask(pmsTask);
    pmsTask.enable();
}

void pmsSensorWorker()
{
    static PMS::DATA data;
    if (pms.readUntil(data, 100))
    {
        Serial.print("PM 1.0 (ug/m3): ");
        Serial.print(data.PM_AE_UG_1_0);
        pm1.add(data.PM_AE_UG_1_0);

        Serial.print(" - PM 2.5 (ug/m3): ");
        Serial.print(data.PM_AE_UG_2_5);
        pm25.add(data.PM_AE_UG_2_5);

        Serial.print(" - PM 10.0 (ug/m3): ");
        Serial.println(data.PM_AE_UG_10_0);
        pm10.add(data.PM_AE_UG_10_0);
    }
}
