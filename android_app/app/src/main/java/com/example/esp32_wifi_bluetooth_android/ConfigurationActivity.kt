package com.example.esp32_wifi_bluetooth_android

import android.Manifest
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import org.json.JSONObject
import java.io.OutputStream
import java.util.*

private var bluetoothSocket: BluetoothSocket? = null
private var outputStream: OutputStream? = null
private val sppUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

class ConfigurationActivity : AppCompatActivity() {
    private lateinit var textViewSelectedDevice: TextView
    private lateinit var editTextSSID: EditText
    private lateinit var editTextPassword: EditText
    private lateinit var editTextDescription: EditText
    private lateinit var buttonSubmit: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_configuration)

        textViewSelectedDevice = findViewById(R.id.textViewSelectedDevice)
        editTextSSID = findViewById(R.id.editTextSSID)
        editTextPassword = findViewById(R.id.editTextPassword)
        editTextDescription = findViewById(R.id.editTextDescription)
        buttonSubmit = findViewById(R.id.buttonSubmit)

        val device = intent.getParcelableExtra<BluetoothDevice>("device")
        if (device == null) {
            Toast.makeText(this, "No Bluetooth device selected", Toast.LENGTH_SHORT).show()
            finish()
        } else {
            textViewSelectedDevice.text = "Selected device: ${device.name} (${device.address})"
        }

        buttonSubmit.setOnClickListener {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                val ssid = editTextSSID.text.toString()
                val password = editTextPassword.text.toString()
                val description = editTextDescription.text.toString()

                // Create a JSON object with the SSID, password, and description
                val json = JSONObject()
                json.put("ssid", ssid)
                json.put("password", password)
                json.put("description", description)

                // Convert the JSON object to a string
                val message = json.toString()

                // Connect to the device and send the message
                device?.let { selectedDevice ->
                    try {
                        // Create a Bluetooth socket and connect to the device
                        bluetoothSocket = selectedDevice.createRfcommSocketToServiceRecord(sppUuid)
                        bluetoothSocket?.connect()

                        // Get the output stream of the socket
                        outputStream = bluetoothSocket?.outputStream

                        // Send the message to the device
                        outputStream?.write(message.toByteArray())
                        outputStream?.flush()

                        Toast.makeText(this, "Sending to ${selectedDevice.name}: $message", Toast.LENGTH_SHORT).show()
                    } catch (e: Exception) {
                        Toast.makeText(this, "Failed to send message: ${e.message}", Toast.LENGTH_SHORT).show()
                    }
                }
            } else {
                Toast.makeText(this, "Bluetooth connect permission is required", Toast.LENGTH_SHORT).show()
            }
        }

    }

    override fun onDestroy() {
        super.onDestroy()
        // Close the Bluetooth socket and output stream
        try {
            outputStream?.close()
            bluetoothSocket?.close()
        } catch (e: Exception) {
            // Handle exception if needed
        }
    }
}



