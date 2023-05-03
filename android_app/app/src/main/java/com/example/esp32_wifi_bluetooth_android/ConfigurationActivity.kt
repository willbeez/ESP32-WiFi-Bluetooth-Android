package com.example.esp32_wifi_bluetooth_android

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattService
import android.bluetooth.BluetoothProfile
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import org.json.JSONObject
import java.util.UUID

class ConfigurationActivity : AppCompatActivity() {
    private lateinit var textViewSelectedDevice: TextView
    private lateinit var editTextSSID: EditText
    private lateinit var editTextPassword: EditText
    private lateinit var editTextDescription: EditText
    private lateinit var buttonSubmit: Button
    private var bluetoothGatt: BluetoothGatt? = null
    private lateinit var device: BluetoothDevice

    // Replace these UUIDs with your actual service and characteristic UUIDs
    private val YOUR_SERVICE_UUID = UUID.fromString("4fafc201-1fb5-459e-8fcc-c5c9c331914b")
    private val YOUR_CHARACTERISTIC_UUID = UUID.fromString("beb5483e-36e1-4688-b7f5-ea07361b26a8")

    private val gattCallback = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
            when (newState) {
                BluetoothProfile.STATE_CONNECTED -> {
                    runOnUiThread { Toast.makeText(this@ConfigurationActivity, "Connected to GATT server", Toast.LENGTH_SHORT).show() }
                    if (ActivityCompat.checkSelfPermission(
                            this@ConfigurationActivity,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return
                    }
                    gatt.discoverServices()
                }
                BluetoothProfile.STATE_DISCONNECTED -> {
                    runOnUiThread { Toast.makeText(this@ConfigurationActivity, "Disconnected from GATT server", Toast.LENGTH_SHORT).show() }
                }
            }
        }

        override fun onServicesDiscovered(gatt: BluetoothGatt, status: Int) {
            // Find the service and characteristic that you want to write data to
            val service: BluetoothGattService? = gatt.getService(YOUR_SERVICE_UUID)
            val characteristic: BluetoothGattCharacteristic? = service?.getCharacteristic(YOUR_CHARACTERISTIC_UUID)

            // If the service and characteristic are found, set up the button submit listener
            characteristic?.let { char ->
                runOnUiThread {
                    buttonSubmit.setOnClickListener { _ ->
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

                        // Write the message to the characteristic
                        char.setValue(message.toByteArray())
                        if (ActivityCompat.checkSelfPermission(
                                this@ConfigurationActivity,
                                Manifest.permission.BLUETOOTH_CONNECT
                            ) != PackageManager.PERMISSION_GRANTED
                        ) {
                            // TODO: Consider calling
                            //    ActivityCompat#requestPermissions
                            // here to request the missing permissions, and then overriding
                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                            //                                          int[] grantResults)
                            // to handle the case where the user grants the permission. See the documentation
                            // for ActivityCompat#requestPermissions for more details.
                            return@setOnClickListener
                        }
                        gatt.writeCharacteristic(char)

                        Toast.makeText(this@ConfigurationActivity, "Sending to ${device.name}: $message", Toast.LENGTH_SHORT).show()
                    }
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_configuration)

        textViewSelectedDevice = findViewById(R.id.textViewSelectedDevice)
        editTextSSID = findViewById(R.id.editTextSSID)
        editTextPassword = findViewById(R.id.editTextPassword)
        editTextDescription = findViewById(R.id.editTextDescription)
        buttonSubmit = findViewById(R.id.buttonSubmit)
        val deviceAddress = intent.getStringExtra("device_address")
        device = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress)

        textViewSelectedDevice.text = "Selected device: ${device.name} (${device.address})"

        // Connect to the device using GATT
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return
        }
        bluetoothGatt = device.connectGatt(this, false, gattCallback)
    }

    override fun onDestroy() {
        super.onDestroy()
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return
        }
        bluetoothGatt?.close()
    }

    private fun hasBluetoothConnectPermission(): Boolean {
        return ActivityCompat.checkSelfPermission(
            this,
            Manifest.permission.BLUETOOTH_CONNECT
        ) == PackageManager.PERMISSION_GRANTED
    }

    private fun requestBluetoothConnectPermission() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(Manifest.permission.BLUETOOTH_CONNECT),
            PERMISSION_REQUEST_BLUETOOTH_CONNECT
        )
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            PERMISSION_REQUEST_BLUETOOTH_CONNECT -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this, "Bluetooth Connect permission granted", Toast.LENGTH_SHORT).show()
                } else {
                    Toast.makeText(this, "Bluetooth Connect permission denied", Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    companion object {
        private const val PERMISSION_REQUEST_BLUETOOTH_CONNECT = 1
    }
}