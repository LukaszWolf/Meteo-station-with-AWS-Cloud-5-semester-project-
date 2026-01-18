/**
 * @file NetworkManager.cpp
 * @brief Implementation of the NetworkManager class.
 */

#include "NetworkManager.h"
#include "SensorManager.h"

static NetworkManager *netInstance = nullptr;

void OnDataRecvWrapper(const uint8_t *mac, const uint8_t *incomingData,
                       int len) {
  memcpy(&Data, incomingData, sizeof(Data));
  lastDataReceivedMs = millis();
  newDataReceived = true;
  screenDataDirty = true;
}

void mqttCallbackWrapper(char *topic, byte *payload, unsigned int len) {
  if (netInstance) {
    netInstance->handleMqttMessage(topic, payload, len);
  }
}

NetworkManager::NetworkManager(SensorManager *sensorMgr)
    : _sensorMgr(sensorMgr), server(80), client(net) {
  netInstance = this;
}

void NetworkManager::begin() {
  prefs.begin("claim", true);
  ownerIdentityId = prefs.getString("ownerId", "");
  prefs.end();

  prefs.begin("net", true);
  String savedSSID = prefs.getString("ssid", "");
  prefs.end();

  if (savedSSID.isEmpty()) {
    startConfigPortal();
  }

  if (!initEspNow()) {
    Serial.println("[NET] ESP-NOW Init Failed");
  }

  if (!tryConnectSaved(1000)) {
    Serial.println("[NET] Started in Local Mode");
  } else {
    WiFi.disconnect();
    initEspNow();
  }
}

void NetworkManager::loop() {
  if (WiFi.status() == WL_CONNECTED && client.connected()) {
    client.loop();
  }

  if (newDataReceived && !_sendingToAws) {
    _sendingToAws = true;
    newDataReceived = false;

    if (tryConnectSaved(3000)) {
      if (connectAWS()) {
        publishToAWS();
        client.loop();
        if (!connectionGood)
          connectionGood = true;
      } else {
        if (connectionGood)
          connectionGood = false;
      }
    } else {
      if (connectionGood)
        connectionGood = false;
    }

    if (!_configPortalActive) {
      client.disconnect();
      WiFi.disconnect();
      delay(50);
      initEspNow();
    }
    _sendingToAws = false;
  }
}

bool NetworkManager::initEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK)
    return false;
  esp_now_register_recv_cb(OnDataRecvWrapper);
  return true;
}

bool NetworkManager::tryConnectSaved(unsigned timeoutMs) {
  connectionGood = false;
  prefs.begin("net", true);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  WiFi.mode(WIFI_AP_STA);

  if (ssid.isEmpty()) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < timeoutMs) {
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFi.setSleep(false);
    connectionGood = true;
    return true;
  } else {
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    return false;
  }
}

String NetworkManager::loadFile(const char *path) {
  File f = LittleFS.open(path, "r");
  if (!f)
    return String();
  String s = f.readString();
  f.close();
  return s;
}

bool NetworkManager::connectAWS() {
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  time_t now = time(nullptr);
  int retries = 0;
  while (now < 1700000000 && retries < 5) {
    delay(200);
    now = time(nullptr);
    retries++;
  }

  if (client.connected())
    return true;

  String ca = loadFile("/certs/AmazonRootCA1.pem");
  String crt = loadFile("/certs/certificate.pem.crt");
  String key = loadFile("/certs/private.pem.key");

  if (ca.isEmpty() || crt.isEmpty() || key.isEmpty()) {
    return false;
  }

  net.setCACert(ca.c_str());
  net.setCertificate(crt.c_str());
  net.setPrivateKey(key.c_str());
  client.setServer(AWS_ENDPOINT, AWS_PORT);
  client.setKeepAlive(60);
  client.setSocketTimeout(2);
  client.setCallback(mqttCallbackWrapper);

  return client.connect(CLIENT_ID);
}

void NetworkManager::publishToAWS() {
  float indoorTemp = _sensorMgr->readIndoorTemp();
  if (!isnan(indoorTemp)) {
    homeTemperatureRead = indoorTemp;
  }

  DateTime rtcNow = rtc.now();
  long long timestamp_ms = ((long long)rtcNow.unixtime() - 3600LL) * 1000LL;

  String payload =
      String("{\"indoorTemperatureRead\":") + String(homeTemperatureRead) +
      ",\"humidityRead\":" + String(Data.humidityRead) +
      ",\"outdoorTemperatureRead\":" + String(Data.outdoorTemperatureRead) +
      ",\"pressureRead\":" + String(Data.pressureRead) +
      ",\"uvIndexRead\":" + String(Data.uvIndexRead) +
      ",\"ts\":" + String(timestamp_ms) + "}";

  String topic = ownerIdentityId.length()
                     ? "users/" + ownerIdentityId + "/stations/" +
                           String(THING_NAME) + "/data"
                     : String("stations/") + THING_NAME + "/data";

  client.publish(topic.c_str(), payload.c_str());
}

void NetworkManager::generateAppConnectionKey() {
  if (!appConnectionKeyReady) {
    uint32_t r1 = esp_random();
    uint32_t r2 = esp_random();
    char buf[17];
    snprintf(buf, sizeof(buf), "%08lx%08lx", (unsigned long)r1,
             (unsigned long)r2);
    AppConnectionKey = String(buf).substring(0, 8);
    appConnectionKeyReady = true;
  }
}

void NetworkManager::startClaimIfNeeded() {
  prefs.begin("claim", true);
  String existingOwner = prefs.getString("ownerId", "");
  prefs.end();

  if (existingOwner.length()) {
    ownerIdentityId = existingOwner;
    return;
  }

  if (!client.connected()) {
    if (!tryConnectSaved())
      return;
    connectAWS();
  }

  String replyTopic = String("devices/") + THING_NAME + "/claim/reply";
  client.subscribe(replyTopic.c_str());
  client.loop();

  generateAppConnectionKey();

  StaticJsonDocument<200> j;
  j["thingName"] = THING_NAME;
  j["nonce"] = AppConnectionKey;
  String body;
  serializeJson(j, body);

  client.publish((String("devices/") + THING_NAME + "/claim/request").c_str(),
                 body.c_str());
}

void NetworkManager::handleMqttMessage(char *topic, byte *payload,
                                       unsigned int len) {
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, payload, len))
    return;

  const char *id = doc["identityId"];
  const char *nonce = doc["nonce"];

  if (!id || !nonce || String(nonce) != AppConnectionKey)
    return;

  ownerIdentityId = String(id);
  prefs.begin("claim", false);
  prefs.putString("ownerId", ownerIdentityId);
  prefs.end();

  client.unsubscribe(
      (String("devices/") + THING_NAME + "/claim/reply").c_str());
}

void NetworkManager::startConfigPortal() {
  _configPortalActive = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Meteo-Setup", "12345678");
  dns.start(53, "*", WiFi.softAPIP());

  server.serveStatic("/setup/", LittleFS, "/setup/")
      .setDefaultFile("index.html");
  server.on("/", HTTP_GET,
            [](AsyncWebServerRequest *r) { r->redirect("/setup/"); });
  server.onNotFound([](AsyncWebServerRequest *r) { r->redirect("/setup/"); });

  server.on("/api/save", HTTP_POST, [this](AsyncWebServerRequest *req) {
    if (!req->hasParam("ssid", true) || !req->hasParam("pass", true)) {
      req->send(400, "application/json", "{\"ok\":false}");
      return;
    }
    String ssid = req->getParam("ssid", true)->value();
    String pass = req->getParam("pass", true)->value();

    this->prefs.begin("net", false);
    this->prefs.putString("ssid", ssid);
    this->prefs.putString("pass", pass);
    this->prefs.end();

    req->send(200, "application/json", "{\"ok\":true}");
    delay(500);
    _configPortalActive = false;
    ESP.restart();
  });

  server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *req) {
    int n = WiFi.scanNetworks();
    DynamicJsonDocument j(1024);
    JsonArray arr = j.to<JsonArray>();
    for (int i = 0; i < n; i++) {
      JsonObject o = arr.add<JsonObject>();
      o["ssid"] = WiFi.SSID(i);
      o["rssi"] = WiFi.RSSI(i);
      o["enc"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    }
    String out;
    serializeJson(j, out);
    req->send(200, "application/json", out);
  });

  server.on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest *req) {
    this->prefs.begin("net", false);
    this->prefs.clear();
    this->prefs.end();
    this->prefs.begin("claim", false);
    this->prefs.clear();
    this->prefs.end();
    req->send(200, "application/json", "{\"ok\":true}");
    delay(300);
    ESP.restart();
  });

  server.begin();
}

bool NetworkManager::isConfigPortalActive() { return _configPortalActive; }
bool NetworkManager::isWifiConnected() { return WiFi.status() == WL_CONNECTED; }
bool NetworkManager::isAwsConnected() { return client.connected(); }