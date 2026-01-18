/**
 * @file UIManager.h
 * @brief Manages the User Interface, screens, and touch interactions.
 */

#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>

#include "Background.h"
#include "Config.h"
#include "Globals.h"
#include "Icon.h"
#include "NetworkManager.h"
#include "SensorManager.h"

#include "autoBrightnessOff_Sprite.h"
#include "autoBrightnessOn_Sprite.h"
#include "settings_sprite.h"
#include "wifiFalse_Sprite.h"
#include "wifitrue_sprite.h"

/**
 * @class UIManager
 * @brief Controls the display, rendering logic, and user input.
 */
class UIManager {
public:
  /**
   * @brief Constructs a new UIManager object.
   * @param sensorMgr Pointer to SensorManager.
   * @param networkMgr Pointer to NetworkManager.
   */
  UIManager(SensorManager *sensorMgr, NetworkManager *networkMgr);

  /**
   * @brief Initializes the display and UI resources.
   */
  void begin();

  /**
   * @brief Main UI loop (updates display and handles touch).
   */
  void update();

  /**
   * @brief Switches the active screen.
   * @param s The target screen enum.
   */
  void changeScreen(SCREEN s);

  // Button Callbacks
  void onBtnGoToSettings();
  void onBtnGoToHome();
  void onBtnGoToWifiConnection();
  void onBtnGoToAppConnection();
  void onBtnSwitchAutoBrightness();

private:
  TFT_eSPI tft;                ///< TFT driver.
  PNG png;                     ///< PNG decoder.
  SensorManager *_sensorMgr;   ///< Pointer to SensorManager.
  NetworkManager *_networkMgr; ///< Pointer to NetworkManager.

  Background *bgHome;     ///< Home screen background.
  Background *bgSettings; ///< Settings screen background.
  Background *bgAccount;  ///< Account/WiFi screen background.

  TFT_eSprite settingsIconSprite; ///< Helper sprite for settings icon.
  SCREEN currentScreen;           ///< Currently active screen.

  uint32_t lastDrawMs = 0;      ///< Timestamp of last UI redraw.
  int lastDrawnMinute = -1;     ///< Last drawn minute value (to avoid redraws).
  int lastDrawnDay = -1;        ///< Last drawn day value.
  bool homeStaticDrawn = false; ///< Flag if static elements are drawn.

  Background *getActiveBackground();
  void drawIconLazy(const char *path, int x, int y, uint16_t bg = TFT_BLACK);
  void updateAutoBrightnessIcon(bool status);
  void updateConnectionIcon(bool status);
  void drawConnectionStatusText();
  void drawAccountConnectionStatusText();
  void drawHomeScreenDynamicData();
  void drawHomeScreenClockAndDate();
};