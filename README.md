# Sensor Data Dashboard

This repository contains an ESP32-based device and a Flask server for collecting data agnostically from sensors, such as temperature and humidity data from a DHT11 sensor. The system receives Wi-Fi credentials via Bluetooth and sends the data to a server via HTTP. The server stores the data in an SQLite database and provides a simple web-based dashboard for visualization. The design allows for the easy addition and integration of other types of sensor data.

## Contents

1. [ESP32 Device Code](#esp32-device-code)
2. [Flask Server Code](#flask-server-code)
3. [Dashboard](#dashboard)
4. [Installation](#installation)
5. [Running the Project](#running-the-project)

## ESP32 Device Code
![image](https://user-images.githubusercontent.com/3200244/230788400-8ed501d1-22ee-4a8f-b222-f12297f3404a.png)


The `single-node.ino` file contains the Arduino code for the ESP32 microcontroller. This code reads temperature and humidity data from a DHT11 sensor, receives Wi-Fi credentials via Bluetooth, and sends the data to the Flask server via HTTP.

### Libraries

The following libraries are required to compile the code:

- WiFi.h
- HTTPClient.h
- BluetoothSerial.h
- ArduinoJson.h
- time.h
- ctime
- iomanip
- DHT.h

### Hardware

The code assumes that a DHT11 sensor is connected to the D4 pin of the ESP32.

## Flask Server Code
![image](https://user-images.githubusercontent.com/3200244/230788442-429ffe64-38c8-48e1-a749-e5069353735f.png)


The `app.py` file contains the Flask server code that receives incoming sensor data via an API, stores it in an SQLite database, and serves a simple dashboard for viewing the data.

### Dependencies

You'll need to install the following Python packages:

- Flask
- Flask-SQLAlchemy

## Dashboard
![image](https://user-images.githubusercontent.com/3200244/230788041-8fd81d92-7451-424e-8cb3-62ed89a7b858.png)

The `index.html` file is a simple HTML dashboard for viewing the sensor data. It fetches the data from the Flask server and displays it in a table.

### Dependencies

The dashboard uses the Tabulator library for rendering the table. The library is loaded from a CDN.

## Installation

1. Install [Python 3](https://www.python.org/downloads/) and [pip](https://pip.pypa.io/en/stable/installing/) if you haven't already.
2. Install the required Python packages by running:
  pip install Flask Flask-SQLAlchemy

3. Install [Arduino IDE](https://www.arduino.cc/en/software) and [configure it for ESP32](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).

4. Install the required Arduino libraries using the Arduino IDE's Library Manager:

- [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)
- [ArduinoJson](https://arduinojson.org/)

## Running the Project

1. Upload the `esp32_device.ino` sketch to your ESP32 using the Arduino IDE.

2. Run the Flask server by executing the following command in the terminal or command prompt:
  python3 server.py

The server will start on `http://0.0.0.0:5000/`.

3. Open a web browser and navigate to `http://localhost:5000/` to view the dashboard.

4. Connect to the ESP32 via Bluetooth using a Bluetooth serial terminal app on your smartphone or computer. Send the Wi-Fi credentials as a JSON string in the following format:
  { "ssid": "your_wifi_ssid", "password": "your_wifi_password" }

The ESP32 will connect to the Wi-Fi network and start sending sensor data to the Flask server every 10 seconds.

5. The dashboard will update with the new sensor data. Refresh the page to see the latest data.

Note: Make sure to replace the `serverUrl` variable in the `esp32_device.ino

## Project Structure

- `single-node.ino`: ESP32 Arduino code for reading sensor data, connecting to Wi-Fi, and sending data to the server.
- `app.py`: Flask server code for handling incoming data, storing it in an SQLite database, and serving the dashboard.
- `index.html`: HTML file for the dashboard, displaying sensor data in a table format.
- `sensor_data.db`: SQLite database file that stores the sensor data (created automatically when the Flask server runs).

## Troubleshooting

1. If the ESP32 cannot connect to the Wi-Fi network, ensure that the provided credentials are correct and that the ESP32 is within range of the Wi-Fi network.
2. If the ESP32 cannot send data to the Flask server, ensure that the server is running, and the IP address and port in the `esp32_device.ino` file are correct.
3. If the dashboard is not displaying any data, ensure that the Flask server is running and that the ESP32 is sending data correctly.

## License

This project is provided under the GNU General Public License v3.0 (GPL-3.0). See the [LICENSE](LICENSE) file for more information.
