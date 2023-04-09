# ESP32 Wi-Fi and Bluetooth Sensor Data Collection

This project consists of an ESP32 microcontroller that collects sensor data and sends it to a Flask server over Wi-Fi or Bluetooth. The server stores the data in a SQLite database and provides an API for accessing the data. An Android app is included to connect to the ESP32 over Bluetooth and send Wi-Fi credentials to the device.

## Components

- ESP32 development board
- DHT11 temperature and humidity sensor
- MQ-2 gas sensor
- Breadboard and jumper wires
- Android smartphone with Bluetooth support

## Setting up the ESP32

1. Install the ESP32 board in the Arduino IDE using the instructions found [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).
2. Install the DHT sensor library by navigating to **Sketch > Include Library > Manage Libraries...** in the Arduino IDE and searching for "DHT sensor library" by Adafruit.
3. Install the PubSubClient library by navigating to **Sketch > Include Library > Manage Libraries...** in the Arduino IDE and searching for "PubSubClient" by Nick O'Leary.
4. Upload the `esp32-wifi-bluetooth.ino` sketch to the ESP32 board.

## Setting up the Flask Server

1. Install the required packages by running `pip install -r requirements.txt`.
2. Run `python app.py` to start the server.

## Using the Android App

1. Pair your Android smartphone with the ESP32 over Bluetooth.
2. Open the app and select the ESP32 from the list of available devices.
3. Enter your Wi-Fi credentials in the provided fields.
4. Press the "Submit" button to send the credentials to the ESP32.

## API Endpoints

### GET /api/sensor_data

Returns an array of all sensor data in the database, ordered by timestamp in descending order.

### POST /api/sensor_data

Accepts JSON data in the following format:

{
  "device_id": "12345",
  "timestamp": "2022-04-08T12:34:56.789Z",
  "data": [
  {
    "key": "temperature",
    "value": 25.5
  },
  {
    "key": "humidity",
    "value": 50.0
  },
  {
    "key": "gas",
    "value": 150
  }]
}

Returns a success message if the data was added to the database, or an error message if an error occurred.

## License

This project is licensed under the MIT License. See the [LICENSE.md](LICENSE.md) file for details.
