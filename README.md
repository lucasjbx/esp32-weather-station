# Estación Meteorológica con ESP32

Este proyecto es una estación meteorológica basada en un ESP32 que muestra datos de un sensor BME280 y de una API pública en una pantalla OLED.

**Este proyecto fue desarrollado con la asistencia de un Large Language Model (LLM) de Google.**

## Características

*   **Hardware:**
    *   ESP32 DevKit V1
    *   Pantalla OLED 128x64 (I2C)
    *   Sensor de temperatura, humedad y presión BME280 (I2C)
*   **Software:**
    *   **Portal Cautivo:** Al primer inicio o tras un reseteo de fábrica, el ESP32 crea un punto de acceso WiFi ("Meteo-Setup") para que el usuario pueda introducir las credenciales de su red local.
    *   **Interfaz Moderna:** La pantalla OLED muestra una interfaz limpia con iconos y fuentes de diferentes tamaños. Todo el texto está en italiano.
    *   **Datos Climáticos Locales y Remotos:**
        *   Muestra la temperatura, humedad y presión obtenidas del sensor local BME280.
        *   Obtiene y muestra el estado del tiempo actual (icono, temperatura y descripción) para Ciro Marina, Italia, desde la API de Open-Meteo.
    *   **Animaciones:** Las pantallas de datos locales y remotos se alternan con una animación de desplazamiento horizontal.
    *   **Gestión de Errores:** Muestra un mensaje de error en la pantalla si no se puede inicializar el sensor BME280.

## Pines I2C

*   **SDA:** GPIO 22
*   **SCL:** GPIO 21

## Configuración y Compilación

Este proyecto está configurado para ser compilado con [PlatformIO](https://platformio.org/).

1.  **Clonar el repositorio:**
    ```bash
    git clone <URL-del-repositorio>
    cd <nombre-del-repositorio>
    ```
2.  **Conectar el ESP32:** Conecta tu placa ESP32 al ordenador mediante USB.
3.  **Compilar y Subir:**
    Abre el proyecto en VSCode con la extensión de PlatformIO y usa la opción "Upload", o ejecuta el siguiente comando en el terminal:
    ```bash
    platformio run --target upload
    ```
    PlatformIO se encargará de descargar las librerías necesarias, compilar el código y subirlo a la placa.

## Librerías Utilizadas

*   `thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays`
*   `adafruit/Adafruit BME280 Library`
*   `tzapu/WiFiManager`
*   `bblanchon/ArduinoJson`
