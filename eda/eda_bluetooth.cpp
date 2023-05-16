/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * Module for bluetooth. Uses secure simple pairing (SSP), because it is perfect*
 * for such a minimal device with only a screen and a binary user input. A part *
 * of code is from a BluetoothSerial library's example.                         */

#include "include/eda_bluetooth.h"
#include "include/eda_display.h"

TaskHandle_t eda_bluetooth_trigger = NULL;
String eda_bluetooth_buffer;

static int connection_time = 7000/portTICK_PERIOD_MS;
static BluetoothSerial BluetoothObject;
bool eda_bluetooth_authentificated = true;

static void _loop(void*);
static void onRequest(uint32_t);
static void onAuth(bool);

///////////////////////////////////////////////////////////////////////////////

void eda_bluetooth_init() {
	BluetoothObject.enableSSP();
	BluetoothObject.onConfirmRequest(onRequest);
	BluetoothObject.onAuthComplete(onAuth);
	BluetoothObject.begin("Espressif Distant Alarm");
    Serial.println("Bluetooth started.");
    xTaskCreate(_loop, "BT_TASK", 4096, NULL, 1, &eda_bluetooth_trigger);
}

static void _loop(void *params) { while (1) {
    vTaskDelay(200/portTICK_PERIOD_MS);
    if (!eda_bluetooth_authentificated) {
        ulTaskNotifyTake(pdTRUE, connection_time);
        eda_stopFlag = true;
        Serial.println(digitalRead(TILT_SENSOR));
        BluetoothObject.confirmReply(digitalRead(TILT_SENSOR) == TILT_ACTIVE);
    } else {
        if (BluetoothObject.available() || Serial.available()) {
            xTaskNotifyGive(eda_tilt_sensor_trigger);
            eda_bluetooth_buffer += BluetoothObject.readString();
            eda_bluetooth_buffer += Serial.readString();
            eda_bluetooth_buffer.trim();
            Serial.print("Read from bluetooth/serial: ");
            Serial.println(eda_bluetooth_buffer);

            eda_display_print(eda_bluetooth_buffer);
            eda_bluetooth_buffer.clear();
            eda_stopFlag = false;
            xTaskNotifyGive(eda_listener_trigger);
        }
    }
}}

static void onRequest(uint32_t numVal) {
    eda_stopFlag = true;
    eda_bluetooth_authentificated = false;
    String authMessage = String(numVal) + "  WANTS TO PAIR";  
    Serial.println(authMessage);
    eda_display_print(authMessage);
}

static void onAuth(bool success) {
    if (success) {
        eda_bluetooth_authentificated = true;
        Serial.println("PAIRING SUCCESS");
        eda_display_print("PAIRING SUCCESS", false);
    } else {
        Serial.println("PAIRING REJECTED");
        eda_display_print("PAIRING REJECTED", false);
    }
}
