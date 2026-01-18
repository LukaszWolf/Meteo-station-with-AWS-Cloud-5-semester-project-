/**
 * @file Globals.h
 * @brief Declaration of global variables and shared data structures.
 */

#pragma once
#include <Arduino.h>
#include <RTClib.h>

/**
 * @struct struct_message
 * @brief Data structure for ESP-NOW sensor data.
 */
typedef struct struct_message {
  uint8_t humidityRead; ///< Humidity reading.
  int16_t
      outdoorTemperatureRead; ///< Outdoor temp value * 10 (e.g., 255 = 25.5C).
  uint16_t pressureRead;      ///< Atmospheric pressure reading.
  uint8_t uvIndexRead;        ///< UV Index value * 10.
} struct_message;

// --- Global Variables ---
extern struct_message Data;       ///< Latest sensor data from ESP-NOW.
extern float homeTemperatureRead; ///< Latest local indoor temperature.
extern volatile bool
    newDataReceived; ///< Flag indicating new ESP-NOW data arrived.
extern volatile bool screenDataDirty; ///< Flag indicating UI needs an update.
extern volatile uint32_t
    lastDataReceivedMs; ///< Timestamp of last data reception.

extern bool connectionGood;     ///< WiFi/AWS connection status flag.
extern bool autoBrightness;     ///< Auto-brightness mode status.
extern String ownerIdentityId;  ///< Cloud user identity ID for ownership.
extern String AppConnectionKey; ///< Generated claiming nonce for app pairing.

extern RTC_DS3231 rtc; ///< RTC instance.
extern DateTime now;   ///< Current system time (updated in loop).