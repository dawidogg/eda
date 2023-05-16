#include "eda_pin_definitions.h"
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <freertos/FreeRTOS.h>

#ifndef EDA_BLUETOOTH
#define EDA_BLUETOOTH

void eda_bluetooth_init();

extern String eda_bluetooth_buffer;
extern TaskHandle_t eda_bluetooth_trigger;
extern bool eda_bluetooth_authentificated;

extern TaskHandle_t eda_listener_trigger;
extern bool eda_stopFlag;
extern TaskHandle_t eda_tilt_sensor_trigger;

#endif
