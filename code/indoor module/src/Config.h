/**
 * @file Config.h
 * @brief System-wide configuration constants and settings.
 */

#pragma once
#include <Arduino.h>

// --- Hardware Pins ---
#define FOTORESISTOR_PIN 32 ///< Analog input pin for photoresistor
#define ONE_WIRE_BUS 33     ///< Pin for OneWire (DS18B20)
#define I2C_SDA 25          ///< I2C SDA Pin
#define I2C_SCL 26          ///< I2C SCL Pin
#define TFT_LED_PIN 2       ///< TFT backlight control pin

// --- Fonts ---
#define EXTRA_SMALL_FONT_NAME "fonts/Lato-Regular-18"
#define SMALL_FONT_NAME "fonts/Lato-Regular-24"
#define TIME_FONT_NAME "fonts/Lato-Regular-92"
#define MEDIUM_BOLD_FONT_NAME "fonts/Lato-Semibold-32"

// --- Background Images ---
#define BG_HOME_PATH "/images/main_screen-min.png"
#define BG_SETTINGS_PATH "/images/settings_screen-min.png"
#define BG_ACCOUNT_PATH "/images/app-connecting-screen-min.png"

// --- AWS IoT Config ---
const char *const AWS_ENDPOINT =
    "an7hi8lzvqru3-ats.iot.eu-north-1.amazonaws.com";
const int AWS_PORT = 8883;
const char *const CLIENT_ID = "station-001";
const char *const THING_NAME = "station-001";

// --- Application States ---
/**
 * @enum SCREEN
 * @brief Enumeration of available UI screens.
 */
typedef enum {
  HOME_SCREEN,
  SETTINGS_SCREEN,
  APP_CONNECTION_SCREEN,
  WIFI_CONNECTION_SCREEN
} SCREEN;