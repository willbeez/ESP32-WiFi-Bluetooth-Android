#include "BluetoothSerial.h"
#include "WiFi.h"

BluetoothSerial SerialBT;

String receivedData = "";
String ssid = "";
bool ssidtf = false;
String password = "";
bool wifiConnected = false;
int bothSSIDandPass = 0;
char receivedChar = 'a';
bool ssidStart = false;

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
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  Serial.println("Bluetooth device started, waiting for connections...");
}

void loop() {
  // Clear the SSID and password variables before receiving new data
  ssid = "";
  password = "";
  bothSSIDandPass = 0;

  // Check if there is data received from Bluetooth
  while (SerialBT.available()) {
    receivedChar = SerialBT.read();
    delay(50);

    if (receivedChar == '<') {
      ssidStart = true;
      continue;
    }

    if (ssidStart) {
      if (receivedChar == ',') {
        bothSSIDandPass++;
        receivedData.trim();
      } else if (receivedChar == '>') {
        password = receivedData;
        receivedData = "";
        receivedData.trim();
        ssidStart = false; // Reset the ssidStart flag
        break; // Exit the while loop to process the SSID and password
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

  if (ssid != "" && password != "") {
    // Check if received data contains SSID and password
    Serial.print("Connecting to WiFi network: ");
    Serial.print(ssid);
    Serial.print(", Password: ");
    Serial.println(password);
    wifiConnected = connectToWiFi(ssid, password);

    if (wifiConnected) {
      Serial.println("WiFi Connected");
      // Add your code here to do something when WiFi is connected
      // In this example, we'll just print the local IP address to the Serial Monitor
      Serial.print("Local IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi Connection Failed");
    }

    // Clear the received data
    ssid = "";
    password = "";
  }
}