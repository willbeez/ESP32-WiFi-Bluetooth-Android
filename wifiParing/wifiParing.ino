// Import necessary libraries for Bluetooth and WiFi functionality
#include "BluetoothSerial.h"
#include "WiFi.h"

BluetoothSerial SerialBT; // Create a BluetoothSerial instance

// Initialize variables for storing received data and connection status
String receivedData = "";
String ssid = "";
bool ssidtf = false;
String password = "";
bool wifiConnected = false;
int bothSSIDandPass = 0;
char receivedChar = 'a';
bool ssidStart = false;

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

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Set the Bluetooth device name
  Serial.println("Bluetooth device started, waiting for connections...");
}

void loop() {
  // Reset the SSID and password variables for new data
  ssid = "";
  password = "";
  bothSSIDandPass = 0;

  // Read data from the Bluetooth connection
  while (SerialBT.available()) {
    receivedChar = SerialBT.read();
    delay(50);

    if (receivedChar == '<') {
      ssidStart = true;
      continue;
    }

    // Parse the received data for SSID and password
    if (ssidStart) {
      if (receivedChar == ',') {
        bothSSIDandPass++;
        receivedData.trim();
      } else if (receivedChar == '>') {
        password = receivedData;
        receivedData = "";
        receivedData.trim();
        ssidStart = false; // Reset the ssidStart flag
        break; // Exit the loop after receiving complete SSID and password
      } else {
        receivedData += receivedChar;
      }

      if (bothSSIDandPass == 1 && receivedChar == ',' && !ssidtf) {
        ssid = receivedData;
        ssidtf = true;
        receivedData = "";
      }
    }
  }

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

    // Clear the received data for the next iteration
    ssid = "";
    password = "";
  }
}
