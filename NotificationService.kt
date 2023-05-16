package com.franz.notificationlistenertutorial

import android.app.Notification
import android.bluetooth.BluetoothAdapter
import android.content.Context
import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log
import java.io.IOException
import java.util.*

/**
 * Created by Franz Andel on 03/06/20.
 * Android Engineer
 *
 * Edited by Denis Davidoglu using ChatGPT
 */

class NotificationService: NotificationListenerService() {

    private lateinit var context: Context

    val messengers = listOf(
        "com.whatsapp",
        "com.instagram.android",
        "com.facebook.orca",
        "org.telegram.messenger",
        "com.google.android.gm",
        "com.samsung.android.messaging",
        "com.discord",
        "com.franz.notificationlistenertutorial"
    )

    override fun onCreate() {
        super.onCreate()
        context = applicationContext
    }

    override fun onListenerConnected() {
        super.onListenerConnected()
        Log.d("1234", "onListenerConnected")
    }

    override fun onListenerDisconnected() {
        super.onListenerDisconnected()
        Log.d("1234", "onListenerDisconnected")
    }

    override fun onNotificationPosted(sbn: StatusBarNotification?) {
        super.onNotificationPosted(sbn)
        // PostTime in milliseconds
        val postTime = sbn?.postTime
        val extras = sbn?.notification?.extras
        val notificationTitle = extras?.getString(Notification.EXTRA_TITLE)
        val notificationDescription = extras?.getString(Notification.EXTRA_TEXT)
        // For Notification with Big Text
        val notificationBigDescription = extras?.getString(Notification.EXTRA_BIG_TEXT)

        Log.d("1234", "Posted Time : $postTime")
        Log.d("1234", "Posted Package Name : ${sbn?.packageName}")
        Log.d("1234", "Posted notificationTitle : $notificationTitle")

        if (sbn?.packageName in messengers)
            sendViaBluetooth(notificationTitle);
    }

    override fun onNotificationRemoved(sbn: StatusBarNotification?) {
        super.onNotificationRemoved(sbn)
        Log.d("1234", "Removed Package Name : ${sbn?.packageName}")
    }

    private fun sendViaBluetooth(notificationTitle: String?) {
        val bluetoothAdapter = BluetoothAdapter.getDefaultAdapter()

        if (bluetoothAdapter == null) {
            Log.d("1234", "Bluetooth is not supported on this device")
            return
        }

        val deviceName = "Espressif Distant Alarm"
        val device = bluetoothAdapter.bondedDevices.find { it.name == deviceName }

        if (device == null) {
            Log.d("1234", "Bluetooth device '$deviceName' is not paired")
            return
        }

        val message = "$notificationTitle"

        val socket = device.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))

        try {
            socket.connect()

            val outputStream = socket.outputStream
            outputStream.write(message.toByteArray())

            Log.d("1234", "Notification sent via Bluetooth: $message")
        } catch (e: IOException) {
            Log.e("1234", "Failed to send notification via Bluetooth", e)
        } finally {
            socket.close()
        }
    }
}