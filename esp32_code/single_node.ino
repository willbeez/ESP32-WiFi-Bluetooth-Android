#include <WiFi.h>
#include <HTTPClient.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <DHT.h>

#define DHTPIN 4 // Pin D4
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

BluetoothSerial SerialBT;
String ssid = "";
String password = "";

// Replace with your desired server URL
const char *serverUrl = "http://192.168.1.165:5000/api/sensor_data";
const char *device_id = "ESP32_001";  // Add a unique device ID for this ESP32

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

  return true;
}

String getIsoTimeString() {
  time_t now;
  struct tm timeinfo;
  char iso_time[21];

  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(iso_time, sizeof(iso_time), "%Y-%m-%dT%H:%M:%S", &timeinfo);

  return String(iso_time);
}


void connectToWiFi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void sendDataToServer(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    // Get the current timestamp
    String timestamp = getIsoTimeString();

    // Prepare the JSON payload
    String payload = "{";
    payload += "\"device_id\":\"" + String(device_id) + "\",";  // Include the device_id in the payload
    payload += "\"timestamp\":\"" + timestamp + "\",";
    payload += "\"data\":[";  // Start data array
    payload += "{\"key\":\"temperature\",\"value\":" + String(temperature) + "},"; // Add temperature object
    payload += "{\"key\":\"humidity\",\"value\":" + String(humidity) + "}"; // Add humidity object
    payload += "]";  // End data array
    payload += "}";
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



void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Single_Node");
  Serial.println("Bluetooth device started, waiting for connections...");

  dht.begin(); // Initialize DHT sensor
}

void loop() {
  // Check for incoming JSON data via Bluetooth
  if (SerialBT.available()) {
    String receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();

    String ssid, password;
    if (parseJsonData(receivedData, ssid, password)) {
      // Connect to Wi-Fi using the received credentials
      if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi(ssid.c_str(), password.c_str());
      }
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
  }
  delay(10000); // Wait for 10 seconds before sending the next set of data
}
