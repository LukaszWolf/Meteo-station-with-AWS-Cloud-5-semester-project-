/**
 * @file UIManager.cpp
 * @brief Implementation of the UIManager class.
 */

#include "UIManager.h"

UIManager *uiInstance = nullptr;

void wrapperGoToSettings(uint8_t, int16_t, int16_t) {
  if (uiInstance)
    uiInstance->onBtnGoToSettings();
}
void wrapperGoToHome(uint8_t, int16_t, int16_t) {
  if (uiInstance)
    uiInstance->onBtnGoToHome();
}
void wrapperSwitchAutoBrightness(uint8_t, int16_t, int16_t) {
  if (uiInstance)
    uiInstance->onBtnSwitchAutoBrightness();
}
void wrapperGoToAppConnection(uint8_t, int16_t, int16_t) {
  if (uiInstance)
    uiInstance->onBtnGoToAppConnection();
}
void wrapperGoToWifiConnection(uint8_t, int16_t, int16_t) {
  if (uiInstance)
    uiInstance->onBtnGoToWifiConnection();
}

UIManager::UIManager(SensorManager *sensorMgr, NetworkManager *networkMgr)
    : tft(), settingsIconSprite(&tft), _sensorMgr(sensorMgr),
      _networkMgr(networkMgr) {
  uiInstance = this;
  currentScreen = HOME_SCREEN;
}

void UIManager::begin() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  Serial.printf("[UI] Screen initialized: %dx%d\n", tft.width(), tft.height());

  bgHome = new Background(BG_HOME_PATH);
  bgSettings = new Background(BG_SETTINGS_PATH);
  bgAccount = new Background(BG_ACCOUNT_PATH);

  bgHome->addButton(Button(1, 430, 320, 50, 50, wrapperGoToSettings));

  bgSettings->addButton(
      Button(10, 262, 238, 30, 30, wrapperSwitchAutoBrightness));
  bgSettings->addButton(Button(11, 0, 320, 40, 40, wrapperGoToHome));
  bgSettings->addButton(Button(12, 0, 40, 40, 40, wrapperGoToAppConnection));
  bgSettings->addButton(Button(13, 440, 40, 40, 40, wrapperGoToWifiConnection));

  bgAccount->addButton(Button(20, 0, 320, 40, 40, wrapperGoToHome));
  bgAccount->addButton(Button(21, 430, 320, 50, 50, wrapperGoToSettings));

  changeScreen(HOME_SCREEN);
}

void UIManager::update() {
  uint32_t ms = millis();

  uint16_t tx = 0, ty = 0;
  bool touched = tft.getTouch(&tx, &ty);
  Background *activeBg = getActiveBackground();
  if (activeBg) {
    activeBg->handleTouch(touched ? (int16_t)tx : -1,
                          touched ? (int16_t)ty : -1, touched);
  }

  if (lastDataReceivedMs != 0 && (ms - lastDataReceivedMs > 120000UL)) {
    if (connectionGood) {
      connectionGood = false;
      updateConnectionIcon(false);

      if (currentScreen == SETTINGS_SCREEN)
        changeScreen(SETTINGS_SCREEN);
    }
  }

  if (currentScreen == HOME_SCREEN && screenDataDirty) {
    drawHomeScreenDynamicData();
    screenDataDirty = false;
  }

  if (ms - lastDrawMs >= 2000) {
    lastDrawMs = ms;

    if (currentScreen == APP_CONNECTION_SCREEN ||
        currentScreen == WIFI_CONNECTION_SCREEN) {
      updateConnectionIcon(_networkMgr->isWifiConnected());
    } else if (currentScreen == SETTINGS_SCREEN) {
      updateConnectionIcon(connectionGood);
    } else if (currentScreen == HOME_SCREEN) {
      updateConnectionIcon(connectionGood);
      drawHomeScreenClockAndDate();
    }
  }
}

void UIManager::changeScreen(SCREEN s) {
  now = rtc.now();
  currentScreen = s;
  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;

  Background *bg = getActiveBackground();
  if (!bg->draw(tft, png, true)) {
    Serial.println("[UI] Failed to draw background PNG");
    return;
  }

  switch (s) {
  case HOME_SCREEN: {
    lastDrawnMinute = -1;
    lastDrawnDay = -1;

    drawHomeScreenClockAndDate();

    drawIconLazy("/images/out_temp134x52_11.png", 93, 188, TFT_BLACK);
    drawIconLazy("/images/in_temp134x52.png", 254, 188, TFT_BLACK);
    drawIconLazy("/images/hum_press294x34.png", 93, 247, TFT_BLACK);

    settingsIconSprite.createSprite(30, 30);
    settingsIconSprite.setSwapBytes(true);
    settingsIconSprite.pushImage(0, 0, 30, 30, settings_sprite);
    settingsIconSprite.pushSprite(440, 5, TFT_BLACK);
    settingsIconSprite.deleteSprite();

    screenDataDirty = true;
    updateConnectionIcon(connectionGood);
    homeStaticDrawn = false;
    break;
  }

  case APP_CONNECTION_SCREEN: {

    settingsIconSprite.createSprite(30, 30);
    settingsIconSprite.setSwapBytes(true);
    settingsIconSprite.pushImage(0, 0, 30, 30, settings_sprite);
    settingsIconSprite.pushSprite(440, 5, TFT_BLACK);
    settingsIconSprite.deleteSprite();

    updateConnectionIcon(connectionGood);

    tft.loadFont(MEDIUM_BOLD_FONT_NAME);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("POŁĄCZ Z APLIKACJĄ", cx, cy - 130);
    tft.unloadFont();

    tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString("1. Zarejestruj się w aplikacji:", cx - 72, cy - 90);
    tft.drawString("http://vercel.meteo-app/register/", cx - 30, cy - 65);
    tft.drawString("2. Przejdź do zakładki 'Parowanie':", cx - 45, cy - 40);

    if (AppConnectionKey.length() > 0) {

      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.drawString("Twój kod parowania:", cx, cy + 10);

      tft.loadFont(MEDIUM_BOLD_FONT_NAME);
      tft.drawString(AppConnectionKey, cx, cy + 45);
      tft.unloadFont();

      tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLUE);
      tft.drawString("(Wpisz ten kod w aplikacji)", cx, cy + 75);

    } else if (ownerIdentityId.length() > 0) {

      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.drawString("Urządzenie jest już powiązane", cx, cy + 10);
      tft.drawString("z Twoim kontem.", cx, cy + 35);

    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString("3. Kliknij przycisk poniżej,", cx - 75, cy + 10);
      tft.drawString("aby wygenerować kod.", cx - 90, cy + 35);
    }
    tft.unloadFont();
    break;
  }

  case WIFI_CONNECTION_SCREEN: {

    tft.loadFont(MEDIUM_BOLD_FONT_NAME);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("POŁĄCZ Z WIFI", cx, cy - 130);
    tft.unloadFont();

    settingsIconSprite.createSprite(30, 30);
    settingsIconSprite.setSwapBytes(true);
    settingsIconSprite.pushImage(0, 0, 30, 30, settings_sprite);
    settingsIconSprite.pushSprite(440, 5, TFT_BLACK);
    settingsIconSprite.deleteSprite();

    tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);
    tft.drawString("1. Połącz się do sieci Meteo-Setup", cx - 39, cy - 80);
    tft.drawString("2. Jeśli nie zostaniesz automatycznie ", cx - 29, cy - 55);
    tft.drawString("przekierowany do portalu konfiguracyjnego ", cx - 1,
                   cy - 30);
    tft.drawString("wpisz w przeglądarce: ", cx - 83, cy - 5);
    tft.drawString("http://setup.meteo/", cx - 93, cy + 20);
    tft.unloadFont();

    updateConnectionIcon(connectionGood);

    if (!_networkMgr->isConfigPortalActive()) {
      _networkMgr->startConfigPortal();
    }
    break;
  }

  case SETTINGS_SCREEN: {

    tft.loadFont(MEDIUM_BOLD_FONT_NAME);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("USTAWIENIA", cx, cy - 130);
    tft.unloadFont();

    tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);
    tft.drawString("Jasność automatyczna", cx - 85, cy - 78);
    tft.drawString("Jasność: ", cx - 140, cy - 52);
    tft.drawString("Status Wifi/Połączenia z stacją: ", cx - 49, cy - 26);
    tft.drawString("Połącz z Wifi", cx - 83, cy + 116);
    tft.drawString("Połącz z aplikacją", cx + 81, cy + 116);
    tft.unloadFont();

    drawConnectionStatusText();
    drawAccountConnectionStatusText();
    updateConnectionIcon(connectionGood);
    updateAutoBrightnessIcon(autoBrightness);
    break;
  }
  default:
    break;
  }
}

void UIManager::drawHomeScreenDynamicData() {
  float t = _sensorMgr->readIndoorTemp();
  if (!isnan(t)) {
    homeTemperatureRead = t;
  }

  String sIn = String(homeTemperatureRead, 1) + " *C";
  String sOut = String(Data.outdoorTemperatureRead / 10.0, 1) + " C";
  String sHP = "Wilg.:" + String(Data.humidityRead) + " %      " +
               String(Data.pressureRead) + " hPa";

  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;

  tft.loadFont(SMALL_FONT_NAME, LittleFS);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextDatum(MC_DATUM);

  drawIconLazy("/images/out_temp134x52_11.png", 93, 188, TFT_BLACK);
  drawIconLazy("/images/in_temp134x52.png", 254, 188, TFT_BLACK);
  drawIconLazy("/images/hum_press294x34.png", 93, 247, TFT_BLACK);

  tft.drawString(sOut, cx - 62, cy + 55);
  tft.drawString(sHP, cx, cy + 104);
  tft.drawString(sIn, cx + 100, cy + 55);

  tft.unloadFont();
}

void UIManager::drawHomeScreenClockAndDate() {
  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;

  now = rtc.now();

  int m = now.minute();
  if (m != lastDrawnMinute) {
    lastDrawnMinute = m;

    drawIconLazy("/images/time295x111.png", 93, 67, TFT_BLACK);

    String timeStr = "";
    timeStr += String(now.hour()) + ":";
    if (now.minute() < 10)
      timeStr += "0";
    timeStr += String(now.minute());

    tft.loadFont(TIME_FONT_NAME, LittleFS);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(timeStr, cx, cy - 33);
    tft.unloadFont();
  }

  int d = now.day();
  if (d != lastDrawnDay || lastDrawnDay == -1) {
    lastDrawnDay = d;

    drawIconLazy("/images/date265x29.png", 110, 33, TFT_BLACK);

    const char *daysOfWeek[] = {"Niedziela", "Poniedzialek", "Wtorek", "Sroda",
                                "Czwartek",  "Piatek",       "Sobota"};
    int dayIdx = now.dayOfTheWeek();
    if (dayIdx < 0 || dayIdx > 6)
      dayIdx = 0;

    String dateDayStr = String(now.day()) + "." + String(now.month()) + "." +
                        String(now.year()) + ", " + String(daysOfWeek[dayIdx]);

    tft.loadFont(SMALL_FONT_NAME, LittleFS);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(dateDayStr, cx, cy - 110);
    tft.unloadFont();
  }
}

void UIManager::drawIconLazy(const char *path, int x, int y, uint16_t bg) {
  Icon ic(&tft, path, 0, 0, 0);
  if (ic.loadFromFS()) {
    ic.draw(x, y, bg);
  }
}

void UIManager::updateAutoBrightnessIcon(bool status) {
  drawIconLazy("/images/auto_brightness_switch27x26.png", 265, 68, TFT_BLACK);
  tft.setSwapBytes(true);
  if (status) {
    tft.pushImage(267, 70, 24, 24, autoBrightnessOn_Sprite, TFT_BLACK);
  } else {
    tft.pushImage(267, 70, 24, 24, autoBrightnessOff_Sprite, TFT_BLACK);
  }
  tft.setSwapBytes(false);
}

void UIManager::updateConnectionIcon(bool status) {
  tft.setSwapBytes(true);
  if (status) {
    tft.pushImage(400, 5, 30, 30, wifitrue_sprite, TFT_BLACK);
  } else {
    tft.pushImage(400, 5, 30, 30, wifiFalse_Sprite, TFT_BLACK);
  }
  tft.setSwapBytes(false);
}

void UIManager::drawConnectionStatusText() {
  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;

  tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);

  if (connectionGood) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLUE);
    tft.drawString("Połączono z wifi oraz z modułem", cx - 46, cy + 3);
    tft.drawString("zewnętrznym", cx - 122, cy + 30);
  } else {
    tft.setTextColor(TFT_RED, TFT_BROWN);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Brak połączenia! Sprawdź połączenie", cx - 28, cy + 6);
    tft.drawString(" z modułem zewnętrznym oraz siecią Wifi.", cx - 11,
                   cy + 30);
  }
  tft.unloadFont();
}

void UIManager::drawAccountConnectionStatusText() {
  int16_t cx = tft.width() / 2;
  int16_t cy = tft.height() / 2;
  tft.loadFont(EXTRA_SMALL_FONT_NAME, LittleFS);

  if (ownerIdentityId.length() > 0) {
    tft.setTextColor(TFT_DARKGREY, TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Połączono z aplikacją", cx - 55, cy + 54);
  } else {
    tft.setTextColor(TFT_RED, TFT_BROWN);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Nie połączono jeszcze z aplikacją", cx - 45, cy + 54);
  }
  tft.unloadFont();
}

Background *UIManager::getActiveBackground() {
  switch (currentScreen) {
  case HOME_SCREEN:
    return bgHome;
  case SETTINGS_SCREEN:
    return bgSettings;
  case APP_CONNECTION_SCREEN:
    return bgAccount;
  case WIFI_CONNECTION_SCREEN:
    return bgAccount;
  default:
    return bgHome;
  }
}
void UIManager::onBtnGoToSettings() {
  Serial.println("[UI] Action: Go To Settings");
  changeScreen(SETTINGS_SCREEN);
}

void UIManager::onBtnGoToHome() {
  Serial.println("[UI] Action: Go To Home");
  changeScreen(HOME_SCREEN);
}

void UIManager::onBtnSwitchAutoBrightness() {
  autoBrightness = !autoBrightness;
  updateAutoBrightnessIcon(autoBrightness);
}

void UIManager::onBtnGoToWifiConnection() {
  Serial.println("[UI] Action: Go To Wifi Connection");
  changeScreen(WIFI_CONNECTION_SCREEN);
}

void UIManager::onBtnGoToAppConnection() {
  Serial.println("[UI] Action: Go To App Connection");
  changeScreen(APP_CONNECTION_SCREEN);
  _networkMgr->startClaimIfNeeded();
  changeScreen(APP_CONNECTION_SCREEN);
}