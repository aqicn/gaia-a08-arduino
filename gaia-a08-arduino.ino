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

#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>
#include <ArduinoJson.h>

// -----------------------

#define RGB_LED_COUNT 1
#define GPIO_RGB_LED 1
#define GPIO_GREEN_LED 10
#define GPIO_5V_PWR_EN 2
#define GPIO_SDA 8
#define GPIO_SCL 9
#define GPIO_PMS1_RX 4
#define GPIO_PMS2_RX 7

#ifdef USBSerial
#define Serial USBSerial
#endif

// -----------------------

#include "config.h"
#include "src/convertion.hpp"
#include "src/wifi.hpp"
#include "src/accumulator.hpp"
#include "src/met.hpp"
#include "src/pms.hpp"
#include "src/rgb.hpp"
#include "src/led.hpp"
#include "src/scd4x.hpp"
#include "src/uploader.hpp"
#include "src/webserver.hpp"

// -----------------------

Scheduler runner;

void setup()
{
    Serial.begin(115200);
    Serial.println("starting...");

    metSensorInit(runner);
    pmsSensorInit(runner);
    rgbLedInit(runner);
    wifiInit(runner);
    ledInit(runner);
    scd4xSensorInit(runner);

    uploaderInit(runner);
    webServerInit(runner);
}

void loop()
{
    ledLoop();
    rgbLedLoop();
    runner.execute();
}