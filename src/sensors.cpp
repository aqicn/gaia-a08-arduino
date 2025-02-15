#include "sensors.hpp"
#include "main.hpp"

bool getSerialisedSensorData(JsonDocument &doc)
{
    if (!pm25.hasData())
    {
        return false;
    }
    uint64_t efuseMac = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    static char mac[32];
    snprintf(mac, 32, "%llx", efuseMac);

    static char stationID[32];
    uint16_t chip = (uint16_t)(efuseMac >> 32);
    snprintf(stationID, 32, "GAIA-A08-%x", chip);
    Serial.printf("device ID is '%s'\n", stationID);

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
