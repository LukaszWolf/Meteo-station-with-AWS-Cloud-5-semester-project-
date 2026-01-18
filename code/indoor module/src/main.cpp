/**
 * @file main.cpp
 * @brief Entry point of the application. Handles initialization and the main
 * loop.
 */

#include <Arduino.h>
#include <LittleFS.h>
#include <PNGdec.h>

#include "Config.h"
#include "Globals.h"
#include "NetworkManager.h"
#include "SensorManager.h"
#include "UIManager.h"

struct_message telemetryData;             ///< ESP-NOW telemetryData buffer
float homeTemperatureRead = 0.0; ///< Local temperature
volatile bool newDataReceived = false;
volatile bool screenDataDirty = false;
bool connectionGood = false;
bool autoBrightness = false;
String ownerIdentityId = "";
String AppConnectionKey = "";
RTC_DS3231 rtc;
DateTime now;

volatile uint32_t lastDataReceivedMs = 0; ///< Timeout tracker
PNG png;                                  ///< Global PNG decoder instance

SensorManager *sensorMgr = nullptr;
NetworkManager *netMgr = nullptr;
UIManager *uiMgr = nullptr;

/**
 * @brief Setup function.
 */
void setup() {
  Serial.begin(9600);

  if (!LittleFS.begin(true)) {
    Serial.println("[FATAL] LittleFS mount failed");
    TFT_eSPI tft;
    tft.init();
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("LittleFS ERROR!", 10, 10, 4);
    while (true)
      delay(1000);
  }

  Serial.println("[MAIN] Allocating Managers...");

  sensorMgr = new SensorManager();
  netMgr = new NetworkManager(sensorMgr);
  uiMgr = new UIManager(sensorMgr, netMgr);
  sensorMgr->begin();
  netMgr->begin();
  uiMgr->begin();

  Serial.println("[MAIN] System Started Successfully");
}

/**
 * @brief Main Loop.
 */
void loop() {
  if (netMgr)
    netMgr->loop();

  if (sensorMgr)
    sensorMgr->update();

  if (uiMgr)
    uiMgr->update();
  delay(10);
}
