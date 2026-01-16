# Project Overview

This is an ESP32-based weather station project. The goal is to create a device that displays weather information from a BME280 sensor and a public weather API on an OLED display.

## Key Features

*   **Hardware:**
    *   ESP32 devkit1
    *   OLED display 128x64 (I2C)
    *   BME280 sensor (I2C)
*   **Software:**
    *   The ESP32 will create a WiFi access point with a captive portal for the user to enter their WiFi credentials.
    *   The display will have a modern user interface with attractive fonts and graphics, with all text in Italian.
    *   When connected to WiFi, the device will display the local and public IP addresses.
    *   The device will fetch weather data from a free, public API for the city of Ciro Marina, Italy.
    *   The display will show the following information with an animation:
        *   Current time and date.
        *   Weather in Ciro Marina (status, icon, temperature, humidity, pressure).
        *   Local weather data from the BME280 sensor.
    *   Error messages will be displayed if there are issues with the sensor.

## Development Environment

The project is intended to be developed using the Arduino IDE or PlatformIO with the ESP32 toolchain.

### I2C Configuration

*   **SDA:** GPIO 22
*   **SCL:** GPIO 21

## Building and Running

**TODO:** Add instructions on how to build and upload the code to the ESP32.

## Development Conventions

*   All text displayed on the OLED screen must be in Italian.
*   The user interface should be modern and visually appealing.
*   Error handling for the sensor is required.
*   The project should not affect other projects in different folders.
*   Libraries and applications can be installed as needed.
