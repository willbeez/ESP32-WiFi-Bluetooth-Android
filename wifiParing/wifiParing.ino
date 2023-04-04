// Import necessary libraries for Bluetooth, WiFi, and JSON functionality
#include "BluetoothSerial.h"
#include "WiFi.h"
#include <ArduinoJson.h>

BluetoothSerial SerialBT; // Create a BluetoothSerial instance

// Initialize variables for storing received data and connection status
String receivedData = "";
String ssid = "";
String password = "";
bool wifiConnected = false;

// Function to connect to a WiFi network using provided SSID and password
bool connectToWiFi(String ssid, String password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECT_FAILED) {
      return false;
    }
  }
  return true;
}

// Function to handle the received data from the Bluetooth connection
void handleReceivedData(String receivedData) {
  // Parse the received data as JSON
  StaticJsonDocument<256> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, receivedData);

  // If parsing is successful, extract SSID and password
  if (!error) {
    ssid = jsonDoc["ssid"].as<String>();
    password = jsonDoc["password"].as<String>();

    // Connect to the WiFi network if SSID and password are present
    if (ssid != "" && password != "") {
      Serial.print("Connecting to WiFi network: ");
      Serial.print(ssid);
      Serial.print(", Password: ");
      Serial.println(password);
      wifiConnected = connectToWiFi(ssid, password);

      // Perform actions upon successful WiFi connection
      if (wifiConnected) {
        Serial.println("WiFi Connected");
        // Print the local IP address to the Serial Monitor
        Serial.print("Local IP address: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("WiFi Connection Failed");
      }
    }
  } else {
    Serial.println("Failed to parse JSON");
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Set the Bluetooth device name
  Serial.println("Bluetooth device started, waiting for connections...");
}

void loop() {
  // Read data from the Bluetooth connection
  if (SerialBT.available()) {
    receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();

    // Handle the received data
    handleReceivedData(receivedData);

    // Clear the received data for the next iteration
    receivedData = "";
  }
}
