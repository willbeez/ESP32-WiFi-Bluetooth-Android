package com.example.esp32_wifi_bluetooth_android

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.ListView
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.io.OutputStream
import java.util.*

private var bluetoothSocket: BluetoothSocket? = null
private var outputStream: OutputStream? = null
private val sppUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

class MainActivity : AppCompatActivity() {
    private val bluetoothAdapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
    private lateinit var listViewDevices: ListView
    private lateinit var discoveredDevicesAdapter: ArrayAdapter<String>
    private val discoveredDevices: MutableList<BluetoothDevice> = mutableListOf()

    private lateinit var textViewDevice: TextView

    companion object {
        const val PERMISSION_REQUEST_BLUETOOTH_CONNECT = 1
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        listViewDevices = findViewById(R.id.listViewDevices)
        discoveredDevicesAdapter = ArrayAdapter(this, android.R.layout.simple_list_item_1)
        listViewDevices.adapter = discoveredDevicesAdapter

        var selectedDevice: BluetoothDevice? = null

        listViewDevices.setOnItemClickListener { _, _, position, _ ->
            selectedDevice = discoveredDevices[position]
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                // Start ConfigurationActivity with the selected device
                val intent = Intent(this, ConfigurationActivity::class.java)
                intent.putExtra("device", selectedDevice)
                startActivity(intent)
            } else {
                Toast.makeText(this, "Bluetooth connect permission is required", Toast.LENGTH_SHORT).show()
            }
        }

        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        registerReceiver(bluetoothReceiver, filter)

        startDiscovery()
    }

    private val bluetoothReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                BluetoothDevice.ACTION_FOUND -> {
                    val device: BluetoothDevice? = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
                    device?.let {
                        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                            if (!discoveredDevices.contains(it)) {
                                discoveredDevices.add(it)
                                discoveredDevicesAdapter.add(it.name ?: "Unknown Device")
                            }
                        }
                    }
                }
            }
        }
    }

    private fun startDiscovery() {
        requestBluetoothPermissions()
    }

    private fun requestBluetoothPermissions() {
        val permissions = arrayOf(
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.BLUETOOTH_SCAN
        )
        ActivityCompat.requestPermissions(this, permissions, PERMISSION_REQUEST_BLUETOOTH_CONNECT)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == PERMISSION_REQUEST_BLUETOOTH_CONNECT) {
            if (grantResults.isNotEmpty() && grantResults.all { it == PackageManager.PERMISSION_GRANTED }) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    // If permissions granted, start Bluetooth discovery
                    bluetoothAdapter.startDiscovery()
                }
            } else {
                // If permissions are not granted, request permissions again
                Toast.makeText(this, "Bluetooth permissions are required to discover and connect devices", Toast.LENGTH_SHORT).show()
                requestBluetoothPermissions()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceiver(bluetoothReceiver)
    }
}