package com.example.esp32_wifi_bluetooth_android


import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.ArrayAdapter
import android.widget.ListView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.util.*

class MainActivity : AppCompatActivity() {
    private val bluetoothAdapter: BluetoothAdapter = BluetoothAdapter.getDefaultAdapter()
    private lateinit var listViewDevices: ListView
    private lateinit var discoveredDevicesAdapter: ArrayAdapter<String>
    private val discoveredDevices: MutableList<BluetoothDevice> = mutableListOf()

    companion object {
        const val PERMISSION_REQUEST_BLUETOOTH_CONNECT = 1
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        listViewDevices = findViewById(R.id.listViewDevices)
        discoveredDevicesAdapter = ArrayAdapter(this, android.R.layout.simple_list_item_1)
        listViewDevices.adapter = discoveredDevicesAdapter

        listViewDevices.setOnItemClickListener { _, _, position, _ ->
            val selectedDevice = discoveredDevices[position]
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                // Start ConfigurationActivity with the selected device's address
                val intent = Intent(this, ConfigurationActivity::class.java)
                intent.putExtra("device_address", selectedDevice.address)
                startActivity(intent)
            } else {
                Toast.makeText(this, "Bluetooth connect permission is required", Toast.LENGTH_SHORT).show()
            }
        }


        startDiscovery()
    }

    private val bleScanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: android.bluetooth.le.ScanResult?) {
            result?.let {
                val device = it.device
                if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    if (!discoveredDevices.contains(device)) {
                        discoveredDevices.add(device)
                        discoveredDevicesAdapter.add(device.name ?: "Unknown Device")
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
                    // If permissions granted, start BLE discovery
                    val scanner = bluetoothAdapter.bluetoothLeScanner
                    val filters = listOf<ScanFilter>()
                    val settings = ScanSettings.Builder()
                        .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                        .build()
                    scanner.startScan(filters, settings, bleScanCallback)
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
        stopBleScan()
    }

    private fun stopBleScan() {
        val scanner = bluetoothAdapter.bluetoothLeScanner
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_SCAN
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
        scanner.stopScan(bleScanCallback)
    }
}
