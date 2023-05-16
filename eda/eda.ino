/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * The main file of the sketch. Here every module is initialized and some       *
 * crucial control functions along with flags are defined.                      */

#include "include/eda_pin_definitions.h"
#include "include/eda_beep.h"
#include "include/eda_flash.h"
#include "include/eda_display.h"
#include "include/eda_bluetooth.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

TaskHandle_t eda_listener_trigger = NULL;
TaskHandle_t eda_tilt_sensor_trigger = NULL;

bool eda_stopFlag;

void setup();
void eda_listener(void*);
void eda_tilt_sensor_isr();
void eda_tilt_sensor(void*);

///////////////////////////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    eda_pin_init();
    eda_flash_allOff();
    eda_display_init();
    eda_flash_init();
    eda_intro();
    eda_beep_init();
    eda_bluetooth_init();
    attachInterrupt(digitalPinToInterrupt(TILT_SENSOR), 
                    eda_tilt_sensor_isr, (TILT_ACTIVE? RISING : FALLING));
    xTaskCreate(eda_listener, "LISTENER", 2048, NULL, 1, &eda_listener_trigger);
    xTaskCreate(eda_tilt_sensor, "SENSOR", 2048, NULL, 1, &eda_tilt_sensor_trigger);
}

void eda_intro() {
    eda_display_print("ESPRESSIF DISTANT ALARM", false);
    digitalWrite(VIBRATION_MOTOR, HIGH);
    eda_flash_allOn();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    digitalWrite(VIBRATION_MOTOR, LOW);
    vTaskDelay(3000/portTICK_PERIOD_MS);
    eda_display_introAnimation();
    eda_flash_allOff();
}

void eda_listener(void *params) { while (1) {
    eda_display_off();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while (!eda_stopFlag) {
        for (int j = 1; j <= 2 && !eda_stopFlag; j++) {
            eda_beep_setTuneIndex(j);
            for (int i = 0; i < 3 && !eda_stopFlag; i++) {
                eda_beep_trigger();
                eda_flash_trigger();
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            }
            if (eda_stopFlag) break;
            vTaskDelay(2000/portTICK_PERIOD_MS);
        }
    }
}}

void eda_tilt_sensor_isr() {
    BaseType_t pdtrue = pdTRUE;
    vTaskNotifyGiveFromISR(eda_tilt_sensor_trigger, &pdtrue);
}

void eda_tilt_sensor(void *params) { while (1) {
    eda_stopFlag = false;
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    Serial.println("Interrupt!");
    if (eda_bluetooth_authentificated) {
        eda_stopFlag = true;
    } else {
        xTaskNotifyGive(eda_bluetooth_trigger);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
}}

void loop() { vTaskDelay(portMAX_DELAY); }
