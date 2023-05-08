# ESP32 Wi-Fi and Bluetooth Android App

This project is a complete solution for configuring an ESP32-based device with a Wi-Fi network and a description using an Android app. The solution consists of an ESP32 firmware, an Android app with two activities, a Python Flask web application, and an HTML front-end.

## Table of Contents
1. [Requirements](#requirements)
2. [ESP32 Firmware](#esp32-firmware)
3. [Android App](#android-app)
    1. [MainActivity](#mainactivity)
    2. [ConfigurationActivity](#configurationactivity)
4. [Flask Web Application](#flask-web-application)
5. [HTML Front-end](#html-front-end)
6. [Setup and Installation](#setup-and-installation)
7. [Usage](#usage)
8. [Screenshots](#screenshots)

## Requirements

- ESP32 development board
- Android Studio
- Arduino IDE with ESP32 support
- Python with Flask

## ESP32 Firmware

The firmware for the ESP32 device is written in Arduino C++ using the Arduino IDE. The code can be found in `moisturesensor.ino`. The firmware connects the device to a Wi-Fi network using the provided SSID and password, sets up a Bluetooth Low Energy (BLE) server with a custom service, and listens for incoming data to set the description.

## Android App

The Android app is written in Kotlin and consists of two activities, `MainActivity` and `ConfigurationActivity`.

### MainActivity

`MainActivity` is responsible for scanning and displaying available BLE devices. The user can select a device, and the app will proceed to the `ConfigurationActivity`. The code for this activity can be found in `MainActivity.kt`.

### ConfigurationActivity

`ConfigurationActivity` connects to the selected ESP32 device via BLE and sends the Wi-Fi SSID, password, and description. The code for this activity can be found in `ConfigurationActivity.kt`.

## Flask Web Application

The Flask web application is a simple Python server that receives the description from the ESP32 device and serves an HTML front-end to display the received information. The code for the Flask server can be found in `app.py`.

## HTML Front-end

The HTML front-end displays the received description from the ESP32 device. The code for the front-end can be found in `index.html`.

## Setup and Installation

1. Install the required software: Android Studio, Arduino IDE with ESP32 support, and Python with Flask.
2. Load the `moisturesensor.ino` file in the Arduino IDE and upload it to the ESP32 device.
3. Open the Android app project in Android Studio and build the APK.
4. Install the APK on your Android device.
5. Run the Flask server using `python app.py`.

## Usage

1. Power on the ESP32 device.
2. Open the Android app and start scanning for BLE devices.
3. Select the ESP32 device from the list.
4. Enter the Wi-Fi SSID, password, and a description in the `ConfigurationActivity`.
5. Press the "Submit" button to send the information to the ESP32 device.
6. Open a web browser and navigate to the Flask server's URL to view the description.

## Screenshots

- `MainActivity`:
    - (Insert screenshot of the MainActivity here)
- `ConfigurationActivity`:
    - (Insert screenshot of the ConfigurationActivity here)
- Flask Web Application / HTML Front-end:
    - (Insert screenshot of the Flask web app / HTML front-end here)
