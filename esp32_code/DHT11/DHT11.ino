#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>

#define DHTPIN 4 // Pin D4
#define DHTTYPE DHT11 // DHT 11

bool waitingForNewCredentials = false;
String lastReceivedDescription = "";
String receivedDescription = "";

Preferences preferences;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

BluetoothSerial SerialBT;
String ssid = "";
String password = "";

// Replace with your desired server URL
const char *serverUrl = "http://192.168.1.100/api/sensor_data";
const char *serverUrlMetadata = "http://192.168.1.100/api/metadata";

String device_name_prefix = "ESP32_";
String device_name;

typedef struct struct_peerInfo {
  uint8_t mac[6];
  int deviceNumber;
} peerInfo_t;

int deviceNumber;

void onDataReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
  int receivedDeviceNumber = *(int *)data;
  if (receivedDeviceNumber >= deviceNumber) {
    deviceNumber = receivedDeviceNumber + 1;
  }
}

// Rest of the code (parseJsonData, getIsoTimeString, connectToWiFi, sendDataToServer)
bool parseJsonData(String &json, String &ssid, String &password) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  ssid = doc["ssid"].as<String>();
  password = doc["password"].as<String>();
  
  if (!doc["description"].isNull()) {
    receivedDescription = doc["description"].as<String>();
    Serial.print("Parsed description: ");
    Serial.println(receivedDescription);
  } else {
    receivedDescription = "";
    Serial.println("No description found in JSON data.");
  }

  return true;
}

String getIsoTimeString() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  time_t now = epochTime;
  struct tm timeinfo;
  char iso_time[21];

  gmtime_r(&now, &timeinfo); // Use gmtime_r for UTC time
  strftime(iso_time, sizeof(iso_time), "%Y-%m-%dT%H:%M:%S", &timeinfo);

  return String(iso_time);
}


void connectToWiFi(const char *ssid, const char *password) {
  unsigned long startTime;
  unsigned long timeout = 3000; // 3 seconds timeout for the connection attempt
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

      // Check for timeout
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
  } else {
    Serial.print("Failed to connect after ");
    Serial.print(maxRetries);
    Serial.println(" attempts. Giving up.");
    WiFi.disconnect();
    preferences.putString("ssid", "");
    preferences.putString("password", "");
  }
}


void sendDataToServer(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    // Get the current timestamp
    String timestamp = getIsoTimeString();

    // Prepare the JSON payload
    String payload = "{";
    payload += "\"device_name\":\"" + String(device_name) + "\",";  // Include the device_name in the payload
    payload += "\"timestamp\":\"" + timestamp + "\",";
    payload += "\"data\":[";  // Start data array
    payload += "{\"key\":\"temperature\",\"value\":" + String(temperature) + "},"; // Add temperature object
    payload += "{\"key\":\"humidity\",\"value\":" + String(humidity) + "}"; // Add humidity object
    payload += "]";  // End data array
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

void sendMetaDataToServer(const String &description) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrlMetadata);

    // Get the current timestamp
    String timestamp = getIsoTimeString();

    // Prepare the JSON payload
    String payload = "{";
    payload += "\"device_name\":\"" + String(device_name) + "\",";  // Include the device_name in the payload
    payload += "\"timestamp\":\"" + timestamp + "\",";
    payload += "\"metadata\":[";  // Start metadata array
    payload += "{\"key\":\"description\",\"value\":\"" + description + "\"}"; // Add description object
    payload += "]";  // End metadata array
    payload += "}";

    Serial.print("Metadata Payload: ");
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
      Serial.print("Error sending metadata. HTTP error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }  else {
    Serial.println("Error: Not connected to Wi-Fi");
  }
}

String getDeviceName() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "ESP32_%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

void setup() {
  Serial.begin(115200);
  preferences.begin("wifi", false);
  device_name = getDeviceName();

  SerialBT.begin(device_name);
  Serial.println("Bluetooth device started, waiting for connections...");

  dht.begin(); // Initialize DHT sensor
}

void loop() {
  // Check for incoming JSON data via Bluetooth
  if (SerialBT.available()) {
    waitingForNewCredentials = true;

    String receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();

    Serial.print("Received data: ");
    Serial.println(receivedData);

    String ssid, password;
    if (parseJsonData(receivedData, ssid, password)) {
      Serial.println("Parsed data:");
      Serial.print("  SSID: ");
      Serial.println(ssid);
      Serial.print("  Password: ");
      Serial.println(password);

      // Connect to Wi-Fi using the received credentials
      if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi(ssid.c_str(), password.c_str());
      }
      waitingForNewCredentials = false;

      // Check if a new description is received and send it
      Serial.print("Check condition: ");
      Serial.print("waitingForNewCredentials: ");
      Serial.print(waitingForNewCredentials);
      Serial.print(", receivedDescription.length(): ");
      Serial.print(receivedDescription.length());
      Serial.print(", receivedDescription: ");
      Serial.print(receivedDescription);
      Serial.print(", lastReceivedDescription: ");
      Serial.println(lastReceivedDescription);

      if (!waitingForNewCredentials && receivedDescription.length() > 0 && receivedDescription != lastReceivedDescription) {
        sendMetaDataToServer(receivedDescription);
        lastReceivedDescription = receivedDescription;
        receivedDescription = ""; // Clear received description
      }
    } else {
      Serial.println("Error: Failed to parse JSON data");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    // Read actual sensor readings
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      delay(10000); // Add a delay here before trying again
      return;
    }

    sendDataToServer(temperature, humidity);

    delay(10000); // Wait for 10 seconds before sending the next set of data
  } else {
    // If not connected and not waiting for new credentials, try to reconnect
    if (!waitingForNewCredentials) {
      ssid = preferences.getString("ssid", "");
      password = preferences.getString("password", "");

      Serial.print("Stored credentials - SSID: ");
      Serial.println(ssid);
      Serial.print("Stored credentials - Password: ");
      Serial.println(password);

      if (ssid.length() > 0 && password.length() > 0) {
        connectToWiFi(ssid.c_str(), password.c_str());
      }

      // If still not connected, set the flag to wait for new credentials
      if (WiFi.status() != WL_CONNECTED) {
        waitingForNewCredentials = true;
      }
    }
  }
}
