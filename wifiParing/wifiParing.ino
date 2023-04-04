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
//example varables you could import from a JSON file
//float temperature = 0.0;
//float humidity = 0.0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Set the Bluetooth device name
  Serial.println("Bluetooth device started, waiting for connections...");
}

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

bool parseJsonData(String receivedData, StaticJsonDocument<256> &jsonDoc) {
  DeserializationError error = deserializeJson(jsonDoc, receivedData);
  if (error) {
    Serial.println("Failed to parse JSON");
    return false;
  }
  return true;
}

// Function to extract data from the JSON document
void extractDataFromJson(StaticJsonDocument<256> &jsonDoc) {
  ssid = jsonDoc["ssid"].as<String>();
  password = jsonDoc["password"].as<String>();
  //how to import from a JSON file
  //temperature = jsonDoc["temperature"].as<float>();
  //humidity = jsonDoc["humidity"].as<float>();
}

// Function to print the received JSON data
void printJsonData(StaticJsonDocument<256> &jsonDoc) {
  Serial.println("Received JSON data:");
  serializeJsonPretty(jsonDoc, Serial);
  Serial.println();
}

// Function to handle WiFi connections
void handleWiFiConnection() {
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
}

void loop() {
  // Read data from the Bluetooth connection
  if (SerialBT.available()) {
    receivedData = SerialBT.readStringUntil('\n');
    receivedData.trim();

    // Parse the received data as JSON
    StaticJsonDocument<256> jsonDoc;
    if (parseJsonData(receivedData, jsonDoc)) {
      // Extract data from JSON
      extractDataFromJson(jsonDoc);
      
      // Print the received JSON data
      printJsonData(jsonDoc);
      
      // Handle the WiFi connection
      handleWiFiConnection();
    }

    // Clear the received data for the next iteration
    receivedData = "";
  }
}
