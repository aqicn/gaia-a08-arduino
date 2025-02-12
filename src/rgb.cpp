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

#include <WS2812FX.h>
#include "main.hpp"
#include "sensors.hpp"
#include "indicator.hpp"

WS2812FX ws2812fx = WS2812FX(RGB_LED_COUNT, GPIO_RGB_LED, NEO_GRB + NEO_KHZ800);

void rgbLedWorker();
Task rgbTask(1000, TASK_FOREVER, &rgbLedWorker);

const uint32_t aqi_colors[6] = {0x00ff00, 0xffff00, 0xff8000, 0xff4000, 0xff0000, 0xcc00ff};
uint32_t pm25_concentration_to_color(float c);

void rgbLedInit(Scheduler &runner)
{
    ws2812fx.init();
    ws2812fx.setBrightness(5);
    ws2812fx.setSpeed(500);
    ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
    ws2812fx.start();

    runner.addTask(rgbTask);
    rgbTask.enable();
}

// The first 8 seconds, show the rainbow
int rgb_loop_count = 0;

void rgbLedWorker()
{
    if (rgb_loop_count == 8)
    {
        ws2812fx.setMode(FX_MODE_STATIC);
        ws2812fx.setBrightness(0);
    }
    else if (rgb_loop_count > 8)
    {
        if (pm25.hasData())
        {
            uint32_t color = pm25_concentration_to_color(pm25.last());
            ws2812fx.setBrightness(10);
            ws2812fx.setColor(color);
        }
    }
    rgb_loop_count++;
}

void rgbLedLoop()
{
    ws2812fx.service();
}

uint32_t pm25_concentration_to_color(float c)
{
    if (c >= 0 && c < 12.1)
        return aqi_colors[0];
    if (c >= 12.1 && c < 35.5)
        return aqi_colors[1];
    if (c >= 35.5 && c < 55.5)
        return aqi_colors[2];
    if (c >= 55.5 && c < 150.5)
        return aqi_colors[3];
    if (c >= 150.5 && c < 250.5)
        return aqi_colors[4];
    return aqi_colors[5];
}