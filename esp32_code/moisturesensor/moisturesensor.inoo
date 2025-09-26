#include <WiFi.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define MOISTURE_PIN A0
#define AIR_VALUE 4095
#define WATER_VALUE 1741

Preferences preferences;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String ssid = "";
String password = "";

const char *serverUrl = "http://192.168.1.100/api/sensor_data";
const char *serverUrlMetadata = "http://192.168.1.100/api/metadata";

String device_name_prefix = "ESP32_";
String device_name;

bool waitingForBLEConnection = true;
bool isConnectedToWiFi = false;

void setDefaultCredentials() {
  if (ssid == "" && password == "") {
    ssid = "";
    password = "";
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
  }
}

void printStoredCredentials() {
  Serial.print("Stored credentials - SSID: ");
  Serial.println(ssid);
  Serial.print("Stored credentials - Password: ");
  Serial.println(password);
}

bool parseJsonData(const char *json, String &ssid, String &password) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  ssid = doc["ssid"].as<String>();
  password = doc["password"].as<String>();

  return true;
}

String getDeviceName() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "ESP32_%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

String getIsoTimeString() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  time_t now = epochTime;
  struct tm timeinfo;
  char iso_time[21];

  gmtime_r(&now, &timeinfo);
  strftime(iso_time, sizeof(iso_time), "%Y-%m-%dT%H:%M:%S", &timeinfo);

  return String(iso_time);
}

void sendDataToServer(int moisturePercentage) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    String timestamp = getIsoTimeString();

    String payload = "{";
    payload += "\"device_name\":\"" + String(device_name) + "\",";
    payload += "\"timestamp\":\"" + timestamp + "\",";
    payload += "\"data\":[";  
    payload += "{\"key\":\"moisturePercentage\",\"value\":" + String(moisturePercentage) + "}";
    payload += "]";  
    payload += "}";

    Serial.print("Payload: ");
    Serial.println(payload);

        http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error sending data. HTTP error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error: Not connected to Wi-Fi");
  }
}

bool connectToWiFi(const char *ssid, const char *password) {
  unsigned long startTime;
  unsigned long timeout = 6000;
  int maxRetries = 2;
  int retryCount = 0;
  bool connected = false;

  while (!connected && retryCount < maxRetries) {
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");

      if (millis() - startTime > timeout) {
        Serial.println("");
        Serial.print("Connection attempt timed out. Retry ");
        Serial.print(retryCount + 1);
        Serial.print(" of ");
        Serial.print(maxRetries);
        Serial.println(".");
        break;
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
    } else {
      retryCount++;
    }
  }

  if (connected) {
    Serial.println("");
    Serial.print("Connected. IP address: ");
    Serial.println(WiFi.localIP());
    timeClient.begin();
    timeClient.update();
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    return true;
  } else {
    Serial.print("Failed to connect after ");
    Serial.print(maxRetries);
    Serial.println(" attempts. Giving up.");
    WiFi.disconnect();
    preferences.putString("ssid", "");
    preferences.putString("password", "");
    return false;
  }
}

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string receivedData = pCharacteristic->getValue();
    if (receivedData.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(receivedData.c_str());

      String receivedSsid, receivedPassword;
      if (parseJsonData(receivedData.c_str(), receivedSsid, receivedPassword)) {
        Serial.println("Parsed data:");
        Serial.print("  SSID: ");
        Serial.println(receivedSsid);
        Serial.print("  Password: ");
        Serial.println(receivedPassword);

        if (WiFi.status() != WL_CONNECTED) {
          bool connected = connectToWiFi(receivedSsid.c_str(), receivedPassword.c_str());
          waitingForBLEConnection = !connected; // Update the flag based on the connection result
          printStoredCredentials();
        }
      } else {
        Serial.println("Error: Failed to parse JSON data");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  preferences.begin("wifi", false);
  device_name = getDeviceName();
  Serial.println("DeviceName: " + device_name);

  pinMode(MOISTURE_PIN, INPUT);

  BLEDevice::init(device_name.c_str());
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID, 
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_WRITE | 
    BLECharacteristic::PROPERTY_NOTIFY | 
    BLECharacteristic::PROPERTY_INDICATE
  );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE device started, waiting for connections...");
  Serial.println("Bluetooth device started, waiting for connections...");

  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  setDefaultCredentials();
  printStoredCredentials();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    isConnectedToWiFi = true;
    int moistureValue = analogRead(MOISTURE_PIN);
    int moisturePercentage = map(moistureValue, AIR_VALUE, WATER_VALUE, 0, 100);

    sendDataToServer(moisturePercentage);

    delay(10000); // Wait for 10 seconds before sending the next set of data
  } else {
    if (!isConnectedToWiFi && waitingForBLEConnection) {
      ssid = preferences.getString("ssid", "");
      password = preferences.getString("password", "");

      printStoredCredentials(); // Print stored credentials before attempting to connect

      if (ssid != "" && password != "") {
        bool connected = connectToWiFi(ssid.c_str(), password.c_str());
        waitingForBLEConnection = !connected; // Update the flag based on the connection result
      } else {
        Serial.println("No stored Wi-Fi credentials. Waiting for BLE connection...");
        delay(500); // Wait for half a second before checking for incoming data again
      }
    }
  }
}

