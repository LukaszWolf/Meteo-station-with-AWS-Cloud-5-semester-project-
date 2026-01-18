/**
 * @file SensorManager.cpp
 * @brief Implementation of the SensorManager class.
 */

#include "SensorManager.h"

SensorManager::SensorManager() : oneWire(ONE_WIRE_BUS), sensors(&oneWire) {}

void SensorManager::begin() {
  pinMode(FOTORESISTOR_PIN, INPUT);
  pinMode(TFT_LED_PIN, OUTPUT);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!rtc.begin()) {
    Serial.println("[SENS] RTC Not Found");
  }

  sensors.begin();
}

void SensorManager::update() {
  now = rtc.now();
  int brightness = getBrightness();
  analogWrite(TFT_LED_PIN, brightness);
}

float SensorManager::readIndoorTemp() {
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  return (t == DEVICE_DISCONNECTED_C) ? NAN : t;
}

int SensorManager::getBrightness() {
  if (autoBrightness) {
    int read = analogRead(FOTORESISTOR_PIN);
    return max(70, (read - 400) / 16);
  } else {
    return 220;
  }
}