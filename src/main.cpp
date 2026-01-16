#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SSD1306Wire.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WebServer.h> // Explicitly include WebServer
#include <DNSServer.h>   // Explicitly include DNSServer
#include <esp_system.h> // Required for ESP.getResetReason()
#include "icons.h"

// I2C pins
#define I2C_SDA 22
#define I2C_SCL 21

// Factory Reset Button
#define FACTORY_RESET_PIN 4
unsigned long buttonPressStartTime = 0;
const long LONG_PRESS_DURATION = 5000; // 5 seconds

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create an SSD1306 display object
SSD1306Wire display(0x3c, I2C_SDA, I2C_SCL);

// Create a BME280 sensor object
Adafruit_BME280 bme;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
unsigned long lastWeatherUpdate = 0;

enum DisplayState {
  DISPLAY_STATE_LOCAL_DATA,
  DISPLAY_STATE_REMOTE_DATA,
  DISPLAY_STATE_SCROLLING_TO_REMOTE,
  DISPLAY_STATE_SCROLLING_TO_LOCAL
};

DisplayState currentDisplayState = DISPLAY_STATE_LOCAL_DATA;
int scrollXOffset = 0; // Renamed for clarity for horizontal scrolling
const int scrollSpeed = 4; // Pixels per frame, adjusted for horizontal

String weatherStatus = "";
float weatherTemperature = 0.0;
int weatherCode = 0;

String getPublicIP() {
  HTTPClient http;
  http.begin("http://api.ipify.org/");
  int httpCode = http.GET();
  if (httpCode > 0) {
    return http.getString();
  } else {
    return "N/A";
  }
  http.end();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.print("Config Portal IP: ");
  Serial.println(WiFi.softAPIP());

  display.end();
  delay(100);
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  display.drawXbm(52, 0, 24, 24, wifi_icon);
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 26, myWiFiManager->getConfigPortalSSID());
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 48, "Apri 192.168.4.1");

  display.display();
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.printf("Free heap after Serial.begin: %u\n", ESP.getFreeHeap());
  Serial.printf("Reset reason: %d\n", static_cast<int>(esp_reset_reason())); // Add reset reason

  // Initialize I2C bus (always initialized once)
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize the BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    // Basic display init for error message if BME fails early
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.clear();
    display.drawString(0, 0, "Errore sensore BME280");
    display.display();
    while (1); // Halt if BME280 is not found
  }
  Serial.printf("Free heap after bme.begin: %u\n", ESP.getFreeHeap());

  // Initialize Factory Reset Pin
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
  
  // Connect to WiFi
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);

  bool res = wm.autoConnect("Meteo-Setup");

  if(!res) {
    Serial.println("Failed to connect and hit timeout");
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 30, "Connessione fallita!");
    display.display();
    delay(5000);
    ESP.restart();
  }
  else {
    // Successfully connected to WiFi - Display UI
    Serial.println("connected...yeey :)");
    // Initialize display specifically for main loop
    display.init(); // Ensure display is initialized here as well
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.clear();
    display.drawString(0, 0, "Connesso al WiFi");
    display.drawString(0, 12, "IP Locale: " + WiFi.localIP().toString());
    display.drawString(0, 24, "IP Pubblico: " + getPublicIP());
    display.display();
    delay(2000);
    Serial.printf("Free heap after successful WiFi connection: %u\n", ESP.getFreeHeap());

    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
}

void getWeatherData() {
  Serial.println("Fetching weather data...");
  HTTPClient http;
  String url = "https://api.open-meteo.com/v1/forecast?latitude=39.37&longitude=17.13&current_weather=true";
  http.begin(url);
  int httpCode = http.GET();
  Serial.print("HTTP Code: "); Serial.println(httpCode);

  if (httpCode > 0) {
    String payload = http.getString();
    JsonDocument doc;
    deserializeJson(doc, payload);

    weatherTemperature = doc["current_weather"]["temperature"];
    weatherCode = doc["current_weather"]["weathercode"];

    Serial.print("Weather Temperature: "); Serial.println(weatherTemperature);
    Serial.print("Weather Code: "); Serial.println(weatherCode);

    // Map weather code to status
    if (weatherCode == 0) {
      weatherStatus = "Sereno";
    } else if (weatherCode > 0 && weatherCode < 4) {
      weatherStatus = "Poco nuvoloso";
    } else if (weatherCode > 44 && weatherCode < 49) {
      weatherStatus = "Nebbia";
    } else if (weatherCode > 50 && weatherCode < 68) {
        weatherStatus = "Piovere";
    } else if (weatherCode > 70 && weatherCode < 78) {
        weatherStatus = "Nieve";
    } else if (weatherCode > 79 && weatherCode < 100) {
        weatherStatus = "Tempesta";
    } else {
      weatherStatus = "Desconocido";
    }
    Serial.print("Weather Status: "); Serial.println(weatherStatus);
  } else {
    Serial.println("Error on HTTP request: " + String(httpCode));
  }
  http.end();
}

// drawTimeAndDate function removed as per user request (no time/date)
// Separator line also removed from loop()

void drawLocalData(int x_offset) {
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_16);
  display.drawString(2 + x_offset, 0, "Dati locali");

  display.setFont(ArialMT_Plain_10);
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  String tempStr = "Temp: " + String(temperature, 1) + " C";
  display.drawString(2 + x_offset, 20, tempStr);
  String humStr = "Umid: " + String(humidity, 1) + " %";
  display.drawString(2 + x_offset, 35, humStr);
  String presStr = "Pres: " + String(pressure, 0) + " hPa";
  display.drawString(2 + x_offset, 50, presStr);
}

void drawRemoteData(int x_offset) {
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // Draw weather icon (24x24)
  const unsigned char* icon_data = nullptr;
  if (weatherCode == 0) {
    icon_data = sun_icon;
  } else if (weatherCode > 0 && weatherCode < 4) {
    icon_data = partly_cloudy_icon;
  } else if (weatherCode > 44 && weatherCode < 49) {
    icon_data = fog_icon;
  } else if (weatherCode > 50 && weatherCode < 68) {
      icon_data = rain_icon;
  } else if (weatherCode > 70 && weatherCode < 78) {
      icon_data = snow_icon;
  } else if (weatherCode > 79 && weatherCode < 100) {
      icon_data = thunderstorm_icon;
  }

  if (icon_data != nullptr) {
    display.drawXbm(2 + x_offset, 0, 24, 24, icon_data);
  }
  
  display.setFont(ArialMT_Plain_16);
  display.drawString(32 + x_offset, 0, "Ciro Marina");
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(32 + x_offset, 20, weatherStatus);
  String tempStr = String(weatherTemperature, 1) + " C";
  display.drawString(32 + x_offset, 35, tempStr);
}


void loop() {
  // Factory Reset Button Check
  Serial.print("Button state: "); Serial.println(digitalRead(FACTORY_RESET_PIN));
  Serial.print("Press time: "); Serial.println(buttonPressStartTime);

  if (digitalRead(FACTORY_RESET_PIN) == LOW) { // Button is pressed
    if (buttonPressStartTime == 0) { // First detected press
      buttonPressStartTime = millis();
    } else if (millis() - buttonPressStartTime > LONG_PRESS_DURATION) {
      // Long press detected
      Serial.println("Factory Reset initiated!");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 20, "Reset WiFi...");
      display.display();
      delay(1000); // Give time to display message
      WiFiManager wm;
      wm.resetSettings();
      ESP.restart();
    }
  } else { // Button is not pressed
    buttonPressStartTime = 0; // Reset timer
  }

  if (millis() - lastWeatherUpdate > 10000) { // 10 seconds for debugging
    getWeatherData();
    lastWeatherUpdate = millis();
    if (currentDisplayState == DISPLAY_STATE_LOCAL_DATA || currentDisplayState == DISPLAY_STATE_SCROLLING_TO_LOCAL) {
      currentDisplayState = DISPLAY_STATE_SCROLLING_TO_REMOTE;
    } else { // currentDisplayState is DISPLAY_STATE_REMOTE_DATA or DISPLAY_STATE_SCROLLING_TO_REMOTE
      currentDisplayState = DISPLAY_STATE_SCROLLING_TO_LOCAL;
    }
    scrollXOffset = 0; // Reset for horizontal scroll
  }
  
  Serial.print("Current State: "); Serial.println(currentDisplayState);
  Serial.print("Scroll X Offset: "); Serial.println(scrollXOffset);

  display.clear();

  switch (currentDisplayState) {
    case DISPLAY_STATE_LOCAL_DATA:
      drawLocalData(0);
      break;
    case DISPLAY_STATE_REMOTE_DATA:
      drawRemoteData(0);
      break;
    case DISPLAY_STATE_SCROLLING_TO_REMOTE:
      drawLocalData(-scrollXOffset);
      drawRemoteData(SCREEN_WIDTH - scrollXOffset);
      scrollXOffset += scrollSpeed;
      if (scrollXOffset >= SCREEN_WIDTH) {
        currentDisplayState = DISPLAY_STATE_REMOTE_DATA;
        scrollXOffset = 0;
      }
      break;
    case DISPLAY_STATE_SCROLLING_TO_LOCAL:
      drawRemoteData(-scrollXOffset);
      drawLocalData(SCREEN_WIDTH - scrollXOffset);
      scrollXOffset += scrollSpeed;
      if (scrollXOffset >= SCREEN_WIDTH) {
        currentDisplayState = DISPLAY_STATE_LOCAL_DATA;
        scrollXOffset = 0;
      }
      break;
  }
  
  // Add a simple frame around the display
  display.drawRect(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

  display.display();
  delay(50); // 20 FPS during animation
}