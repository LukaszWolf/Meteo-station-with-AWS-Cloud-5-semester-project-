/**
 * @file NetworkManager.h
 * @brief Manages WiFi, ESP-NOW, AWS IoT, and provisioning.
 */

#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <PubSubClient.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "Config.h"
#include "Globals.h"

class SensorManager;

/**
 * @class NetworkManager
 * @brief Handles all network-related operations.
 */
class NetworkManager {
public:
  /**
   * @brief Constructs a new NetworkManager.
   * @param sensorMgr Pointer to the SensorManager instance.
   */
  NetworkManager(SensorManager *sensorMgr);

  /**
   * @brief Initializes network services.
   */
  void begin();

  /**
   * @brief Main network loop handling MQTT and data transmission.
   */
  void loop();

  /**
   * @brief Attempts to connect to saved WiFi credentials.
   * @param timeoutMs Connection timeout in milliseconds.
   * @return true if connected, false otherwise.
   */
  bool tryConnectSaved(unsigned timeoutMs = 3000);

  /**
   * @brief Starts the configuration AP and captive portal.
   */
  void startConfigPortal();

  /**
   * @brief Initiates the device claiming process with AWS.
   */
  void startClaimIfNeeded();

  /**
   * @brief Initializes ESP-NOW protocol.
   * @return true if successful.
   */
  bool initEspNow();

  bool isConfigPortalActive();
  bool isWifiConnected();
  bool isAwsConnected();

private:
  SensorManager *_sensorMgr; ///< Pointer to access sensor data.
  WiFiClientSecure net;      ///< Secure WiFi client for TLS.
  PubSubClient client;       ///< MQTT client.
  AsyncWebServer server;     ///< Web server for provisioning.
  AsyncDNSServer dns;        ///< DNS server for captive portal.
  Preferences prefs;         ///< Storage for WiFi/Claim credentials.

  bool _configPortalActive = false;   ///< Flag indicating active portal.
  bool _sendingToAws = false;         ///< Flag for transmission state.
  bool appConnectionKeyReady = false; ///< Flag for nonce generation state.

  bool connectAWS();
  void publishToAWS();
  void generateAppConnectionKey();
  String loadFile(const char *path);
  void handleMqttMessage(char *topic, byte *payload, unsigned int len);

  friend void mqttCallbackWrapper(char *topic, byte *payload, unsigned int len);
};