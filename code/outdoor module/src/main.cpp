/**
 * @file main.cpp
 * @brief ESP32 Sensor Node (BME280 + UV) transmitting via ESP-NOW.
 * * This program reads data from BME280 (I2C) and an analog UV sensor,
 * then broadcasts the data using ESP-NOW with channel scanning capability.
 * Ideally suited for battery-powered operation using Deep Sleep.
 * * @author Łukasz Wolf
 * @date 2026-01-18
 */

#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ================= HARDWARE DEFINITIONS =================

/** @brief I2C SDA Pin */
const int SDA_PIN = 20;

/** @brief I2C SCL Pin */
const int SCL_PIN = 10;

/** @brief WS2812B NeoPixel control pin */
const int NEOPIXEL_PIN = 5;

/** @brief Analog pin for UV sensor */
const int UV_SENSOR_PIN = 1;

/** @brief I2C address for BME280 sensor */
const uint8_t BMP_ADDR = 0x76;

// ================= CONFIGURATION =================

/** @brief Time to sleep between measurements in seconds */
const uint64_t SLEEP_TIME_SECONDS = 60;

/** @brief Maximum time allowed to try finding a receiver (ms) */
const unsigned long MAX_RETRY_TIME_MS = 20000;

/** @brief Highest allowed WiFi channel */
const uint8_t MAX_WIFI_CHANNEL = 13;

/** * @brief Target MAC address (Broadcast).
 * @note Specific address used: F4:65:0B:E9:77:78
 */
uint8_t broadcastAddress[] = {0xf4, 0x65, 0x0b, 0xe9, 0x77, 0x78};

// ================= GLOBALS =================

/** @brief NeoPixel instance */
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

/** @brief BME280 sensor instance */
Adafruit_BME280 bme;

/** @brief Flag indicating if BME280 initialized successfully */
bool bmpOk = false;

/** * @brief Last successful WiFi channel.
 * @note Stored in RTC memory to survive Deep Sleep.
 */
RTC_DATA_ATTR uint8_t savedChannel = 1;

/**
 * @struct struct_message
 * @brief Data structure for ESP-NOW transmission.
 * @warning Must match the receiver's structure exactly (including padding).
 */
typedef struct struct_message {
  uint8_t humidityRead;           ///< Relative humidity (%)
  int16_t outdoorTemperatureRead; ///< Temperature * 10 (e.g. 255 = 25.5°C)
  uint16_t pressureRead;          ///< Atmospheric pressure (hPa)
  uint8_t uvIndexRead;            ///< UV Raw Value (Clamped to 255)
} struct_message;

struct_message telemetryData;
esp_now_peer_info_t peerInfo;

// Flags for async ESP-NOW callback
volatile bool transmissionFinished = false;
volatile bool transmissionSuccess = false;

// ================= FUNCTIONS =================

/**
 * @brief ESP-NOW send callback function.
 * * Triggered when data is sent. Updates status flags.
 * * @param mac_addr Destination MAC address.
 * @param status Status of transmission (ESP_NOW_SEND_SUCCESS or FAIL).
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  transmissionFinished = true;
  transmissionSuccess = (status == ESP_NOW_SEND_SUCCESS);
}

/**
 * @brief Initializes ESP-NOW and registers the peer.
 * * Sets WiFi mode to Station, disconnects from APs, and adds the broadcast
 * peer.
 */
void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // 0 means "use current channel"
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }
}

/**
 * @brief Changes the WiFi channel.
 * * @param ch Channel number (1-13).
 */
void setEspNowChannel(uint8_t ch) {
  if (ch < 1)
    ch = 1;
  if (ch > MAX_WIFI_CHANNEL)
    ch = MAX_WIFI_CHANNEL;
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
}

/**
 * @brief Reads sensors and populates the `telemetryData` structure.
 * * Reads Temperature, Humidity, Pressure from BME280.
 * Reads UV raw value from Analog Pin.
 * Prints debug info to Serial.
 */
void fillMeasurement() {
  // 1. BME280 Readings
  if (!bmpOk) {
    telemetryData.outdoorTemperatureRead = 0;
    telemetryData.pressureRead = 0;
    telemetryData.humidityRead = 0;
  } else {
    bme.takeForcedMeasurement();
    float temp = bme.readTemperature();
    float pres = bme.readPressure() / 100.0F;
    float hum = bme.readHumidity();

    // Formatting for transmission
    telemetryData.outdoorTemperatureRead = (int16_t)roundf(temp * 10.0f);
    telemetryData.humidityRead = (uint16_t)roundf(hum);
    telemetryData.pressureRead = (uint16_t)roundf(pres);

    // Debug print
    Serial.printf("Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa\n", temp, hum,
                  pres);
  }

  // 2. UV Sensor Readings
  uint16_t uvRaw = analogRead(UV_SENSOR_PIN);
  uint32_t uvMv = analogReadMilliVolts(UV_SENSOR_PIN);

  Serial.printf("UV Raw: %d | Voltage: %d mV\n", uvRaw, uvMv);

  // FIX: Safety clamp to prevent uint8_t overflow (0-255)
  // If your receiver expects raw 0-4095, you must change struct to uint16_t
  if (uvRaw > 255) {
    telemetryData.uvIndexRead = 255;
  } else {
    telemetryData.uvIndexRead = (uint8_t)uvRaw;
  }
}

/**
 * @brief Attempts to send data on a specific WiFi channel using "Burst Mode".
 * * Sends up to 5 packets rapidly to increase the chance of delivery
 * if the receiver is briefly busy.
 * * @param channel The WiFi channel to transmit on.
 * @return true if ACK received (transmission successful).
 * @return false if all attempts failed.
 */
bool trySendOnChannel(uint8_t channel) {
  setEspNowChannel(channel);
  delay(20); // Small delay for radio tuning

  // Burst mode: try 5 times on the same channel
  for (int i = 0; i < 5; i++) {
    transmissionFinished = false;
    transmissionSuccess = false;

    esp_err_t result =
        esp_now_send(broadcastAddress, (uint8_t *)&telemetryData, sizeof(telemetryData));

    if (result == ESP_OK) {
      // Wait for ACK
      unsigned long waitStart = millis();
      while (!transmissionFinished) {
        if (millis() - waitStart > 50)
          break; // 50ms timeout per packet
        yield();
      }

      if (transmissionSuccess) {
        return true; // Success!
      }
    }

    // Short random delay before retry to avoid collision
    delay(10);
  }

  return false; // Failed after retries
}

/**
 * @brief Prepares hardware for sleep and enters Deep Sleep.
 */
void goToDeepSleep() {
  Serial.println("-> Entering Deep Sleep.");
  Serial.flush();

  // Turn off NeoPixel to save power
  pixel.clear();
  pixel.show();

  esp_sleep_enable_timer_wakeup(SLEEP_TIME_SECONDS * 1000000ULL);
  esp_deep_sleep_start();
}

/**
 * @brief Main setup routine.
 * * Runs once per wake-up cycle.
 */
void setup() {
  // Disable brownout detector (Use with caution!)
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  pinMode(UV_SENSOR_PIN, INPUT);

  // Initialize NeoPixel
  pixel.begin();
  pixel.clear();
  pixel.show();

  // Initialize I2C and BME280
  Wire.begin(SDA_PIN, SCL_PIN);
  bmpOk = bme.begin(BMP_ADDR);

  delay(100);

  if (bmpOk) {
    // Configure for forced mode (sleeps between measurements)
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // Temp
                    Adafruit_BME280::SAMPLING_X1, // Pressure
                    Adafruit_BME280::SAMPLING_X1, // Humidity
                    Adafruit_BME280::FILTER_OFF);
    Serial.println("BME280 Initialized.");
  } else {
    Serial.println("Error: BME280 not found.");
  }

  setupEspNow();
  fillMeasurement();

  unsigned long startTime = millis();
  bool sentOk = false;

  // 1. Try saved channel first (Fast connect)
  Serial.printf("Trying saved channel: %d\n", savedChannel);
  if (trySendOnChannel(savedChannel)) {
    Serial.println("Success on saved channel!");
    sentOk = true;
  } else {
    // 2. Scan all channels if saved one failed
    Serial.println("Saved channel failed. Scanning...");

    while (!sentOk && (millis() - startTime < MAX_RETRY_TIME_MS)) {
      for (uint8_t ch = 1; ch <= MAX_WIFI_CHANNEL; ch++) {
        // Break if total timeout reached
        if (millis() - startTime > MAX_RETRY_TIME_MS)
          break;

        Serial.printf("."); // Progress indicator
        if (trySendOnChannel(ch)) {
          Serial.printf("\nFound receiver on CH: %d!\n", ch);
          savedChannel = ch; // Update RTC variable
          sentOk = true;
          break;
        }
      }
      if (!sentOk)
        delay(100);
    }
  }

  goToDeepSleep();
}

void loop() {}
