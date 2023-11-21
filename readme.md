
# GAIA A08 Firmware

This repository is the open-source Arduino firmware used for the [GAIA A08](https://aqicn.org/gaia/a08/).

## Configuration

To start with, you need to update the `config.h` file with your Wi-Fi credential, your aqicn.org Data-Platform token and the station location:

```C
#define TOKEN "dummy-token-for-test-purpose-only"

#define WIFI_SSID "yourNetworkName"
#define WIFI_PASS "yourNetworkPassword"

#define LATITUDE 48.756080
#define LONGITUDE 2.302038
```

You can get your own token from  https://aqicn.org/data-platform/token/


## Libraries

The firmware uses the following libraries:

|Used library        |Version |Path|
|--------------------|--------|----|
|AHT20               |1.0.1   |/Arduino/libraries/AHT20|
|WS2812FX            |1.4.2   |/Arduino/libraries/WS2812FX|
|ArduinoJson         |6.21.3  |/Arduino/libraries/ArduinoJson|
|PMS Library         |1.1.0   |/Arduino/libraries/PMS_Library|
|TaskScheduler       |3.7.0   |/Arduino/libraries/TaskScheduler|
|WiFi                |2.0.0   |/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/WiFi|
|HTTPClient          |2.0.0   |/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/HTTPClient|
|WiFiClientSecure    |2.0.0   |/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/WiFiClientSecure|
|Wire                |2.0.0   |/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/libraries/Wire|
|Sensirion I2C SCD4x |0.4.0   |/Arduino/libraries/Sensirion_I2C_SCD4x
|Sensirion Core      |0.6.0   |/Arduino/libraries/Sensirion_Core

## Compiling

If you use [arduino-cli](https://arduino.github.io/arduino-cli/), you can just use `make` to compile, flash and start the console monitor. You will first need to configure the `SERIAL = /dev/cu.usbmodem101` in the Makefile.

If you use [vs-code Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino), 
you can just select `Arduino: Upload` from the command palette.

## Running

Once the sensor is running with the firmware, you can check your station Data from
https://aqicn.org/data-feed/verification/. 

To see you station, you first need to enter the token you previously got from the aqicn.org data-platform.

## Todo

  - [X] Add support for green LED breathing
  - [x] Add support for CO2 sensor
  - [x] Add support for uploading CO2 sensor temperature and humidity
  - [ ] Add support for Dual PMS configuration
  - [ ] Add support for platform IO
  - [ ] Add support for Wifi Manager

