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

void uploaderWorker();
Task uploaderTask(60 * 1000, TASK_FOREVER, &uploaderWorker);

void uploaderInit(Scheduler &runner)
{
    runner.addTask(uploaderTask);
    uploaderTask.enable();
}

bool uploaderGetCurrentStatus(JsonDocument &doc)
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
        doc["readings"][4]["value"] = humidity.avg();
        doc["readings"][4]["unit"] = "ppm";
    }

    doc["token"] = TOKEN;
    return true;
}

void uploaderResetCurrentStatus()
{
    pm10.reset();
    pm25.reset();
    pm1.reset();
    temperature.reset();
    humidity.reset();
    co2.reset();
}

void uploaderWorker()
{
    // Check WiFi connection status
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Error in WiFi connection");
        return;
    }

    JsonDocument doc;
    if (!uploaderGetCurrentStatus(doc))
    {
        return;
    }
    uploaderResetCurrentStatus();

    Serial.print("Posting: ");
    serializeJson(doc, Serial);
    Serial.println("");

    static char json_body[1024 * 8];
    serializeJson(doc, json_body);

    HTTPClient http;
    http.setUserAgent("GAIA-uploader/1.1");
    http.begin("https://aqicn.org/sensor/upload");
    // http.begin("http://192.168.1.214:88/sensor/upload");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(json_body);

    if (httpResponseCode > 0)
    {

        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    }
    else
    {

        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}