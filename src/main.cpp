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

// -----------------------
#include "main.hpp"

#ifdef USBSerial
#define Serial USBSerial
#endif

// -----------------------
#include "mongoose.h"
#include "config.hpp"
#include "conversion.hpp"
#include "wifi.hpp"
#include "accumulator.hpp"
#include "webserver.hpp"
#include "sensors.hpp"
#include "indicator.hpp"
#include <jled.h>

// -----------------------

Scheduler runner;

auto led = JLed(GPIO_GREEN_LED);
void ledInit(Scheduler &runner)
{
    // To just turn off the Green LED, use:
    // pinMode(GPIO_GREEN_LED, OUTPUT);
    // digitalWrite(GPIO_GREEN_LED, LOW);
    // To make the green LED breathe:
    led.MaxBrightness(100).Breathe(2000).DelayAfter(1000).Forever();
}

void ledLoop()
{
    led.Update();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("starting...");

    metSensorInit(runner);
    pmsSensorInit(runner);
    rgbLedInit(runner);
    ledInit(runner);
    scd4xSensorInit(runner);

    wifiInit(runner);

    uploaderInit(runner);
    webServerInit(runner);
}

void loop()
{
    ledLoop();
    rgbLedLoop();
    runner.execute();
}