#include "sensors.hpp"
#include "main.hpp"
#include <Wire.h>

bool i2c_initialized = false;

void InitializeI2C()
{
    if (i2c_initialized)
    {
        return;
    }

    Wire.begin(GPIO_SDA, GPIO_SCL);
}

bool getMinimalSensorData(JsonDocument &doc)
{
    if (!pm25.hasData())
    {
        return false;
    }

    doc["station"]["id"] = stationID;
    doc["station"]["mac"] = mac;

    doc["station"]["location"]["latitude"] = LATITUDE;
    doc["station"]["location"]["longitude"] = LONGITUDE;
    doc["readings"]["pm1"] = pm1.avg();
    doc["readings"]["pm25"] = pm25.avg();
    doc["readings"]["pm10"] = pm10.avg();
    doc["readings"]["temperature"] = temperature.avg();
    doc["readings"]["humidity"] = humidity.avg();

    if (co2.hasData())
    {
        doc["readings"]["co2"] = round(co2.avg());
    }
    return true;
}

bool getSerialisedSensorData(JsonDocument &doc)
{
    if (!pm25.hasData())
    {
        return false;
    }

    doc["station"]["id"] = stationID;
    doc["station"]["mac"] = mac;

    doc["station"]["location"]["latitude"] = LATITUDE;
    doc["station"]["location"]["longitude"] = LONGITUDE;

    doc["readings"][0]["specie"] = "pm25";
    doc["readings"][0]["value"] = pm25.avg();
    doc["readings"][0]["unit"] = "µg/m3";

    doc["readings"][1]["specie"] = "pm10";
    doc["readings"][1]["value"] = pm10.avg();
    doc["readings"][1]["unit"] = "µg/m3";

    doc["readings"][2]["specie"] = "pm1";
    doc["readings"][2]["value"] = pm1.avg();
    doc["readings"][2]["unit"] = "µg/m3";

    doc["readings"][3]["specie"] = "temperature";
    doc["readings"][3]["value"] = temperature.avg();
    doc["readings"][3]["unit"] = "C";

    doc["readings"][4]["specie"] = "humidity";
    doc["readings"][4]["value"] = humidity.avg();
    doc["readings"][4]["unit"] = "%";

    if (co2.hasData())
    {
        doc["readings"][4]["specie"] = "co2";
        doc["readings"][4]["value"] = round(co2.avg());
        doc["readings"][4]["unit"] = "ppm";
    }

    doc["token"] = TOKEN;
    return true;
}
