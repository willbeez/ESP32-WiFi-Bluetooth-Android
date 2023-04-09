# ESP32-WiFi-Bluetooth-Android

This repository contains an example of an ESP32 program and an Android app that work together to connect the ESP32 to a WiFi network using credentials received over a Bluetooth connection.

## Overview

The ESP32 code sets up a Bluetooth Serial connection and listens for incoming data containing the SSID and password for the WiFi network. The Android app scans for nearby Bluetooth devices, allows the user to input the SSID and password, and then sends the credentials over Bluetooth to the ESP32 device. Once the ESP32 receives the credentials, it attempts to connect to the WiFi network.

## Features

- Android application to discover nearby Bluetooth devices and send data
- ESP32 firmware to receive data over Bluetooth and connect to a Wi-Fi network
- JSON data format for sending and receiving data
- ArduinoJson library for parsing JSON data on the ESP32

## Components

1. **ESP32 Program**: Written in C++ using the Arduino framework. The code sets up a Bluetooth Serial connection, listens for incoming data, and connects to the WiFi network when the SSID and password are received.

2. **Android App**: Written in Kotlin, this app scans for Bluetooth devices, connects to the ESP32 device, and allows the user to input the SSID and password for the WiFi network. The app then sends the credentials over Bluetooth to the ESP32 device.

## Requirements

### Hardware

- ESP32 development board
- Android device with Bluetooth support

### Software

- Arduino IDE with ESP32 board support installed
- Android Studio for building the Android application

## Setup and Usage

### ESP32 Program

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and the [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32).
2. Open the `wifiParing.ino` file in the Arduino IDE and upload the firmware to the ESP32.
3. Connect your ESP32 board to your computer.
4. Select the appropriate board and COM port in the Arduino IDE.
5. Upload the code to the ESP32.

### Android App

1. Open the Android app in [Android Studio](https://developer.android.com/studio).
2. Connect your Android device to your computer.
3. Enable USB debugging on your Android device.
4. Select your Android device in the Android Studio toolbar.
5. Click the "Run" button to build and run the app on your Android device.
6. In the app, select the ESP32 Bluetooth device from the list of available devices.
7. Enter the SSID and password for your WiFi network and click "Submit".

The ESP32 should now receive the credentials over Bluetooth and connect to the specified WiFi network.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.
