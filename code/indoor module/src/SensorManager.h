/**
 * @file SensorManager.h
 * @brief Manages sensors (DS18B20, RTC) and actuators (Display Brightness).
 */

#pragma once

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <RTClib.h>
#include <Wire.h>

#include "Config.h"
#include "Globals.h"

/**
 * @class SensorManager
 * @brief Handles reading sensors and managing hardware state.
 */
class SensorManager {
public:
  SensorManager();

  /**
   * @brief Initializes sensor hardware.
   */
  void begin();

  /**
   * @brief Updates sensor readings and hardware state.
   */
  void update();

  /**
   * @brief Reads the indoor temperature from DS18B20.
   * @return Temperature in Celsius or NAN on error.
   */
  float readIndoorTemp();

private:
  OneWire oneWire;           ///< OneWire interface for DS18B20.
  DallasTemperature sensors; ///< DallasTemp wrapper.

  int getBrightness();
};