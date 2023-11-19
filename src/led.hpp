#include <jled.h>

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