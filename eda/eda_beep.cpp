/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * Module for buzzer and vibration motor control.                               */

#include "include/eda_beep.h"

/* index 0 - length                 *
 * index 1 - note duration          *
 * index 2 - pause between notes    *
 * index 3.. - note frequencies     */
static int tune1[] = {10, 25, 0, 466, 587, 783, 1174, 1479, 880, 739, 587};
static int tune2[] = {18, 20, 0, 880, 1046, 1318, 1567, 1760, 1567, 1318, 
                      1046, 880, 1046, 1318, 1567, 1760, 1567, 1318, 1046};
static int *tune = tune1;
static TaskHandle_t trigger = NULL;

static void beep(int*);
static void _loop(void*);

///////////////////////////////////////////////////////////////////////////////

void eda_beep_init() {
    xTaskCreate(_loop, "BEEP", 1024, NULL, 1, &trigger);
}   

void eda_beep_setTuneIndex(int i) {
    switch (i) {
        case 1: tune = tune1; break;
        case 2: tune = tune2; break;
        default: return;
    }
}

void eda_beep_trigger() {
    xTaskNotifyGive(trigger);
}

static void beep(int *list) {
    for (int i = 3; i <= list[0]; i++) {
        tone(BUZZER, list[i], list[1]);
        vTaskDelay(list[2]/portTICK_PERIOD_MS);
    }
    vTaskDelay(((list[1])*(list[0]-2))/portTICK_PERIOD_MS);
    if (list[0] > 0) {
        digitalWrite(VIBRATION_MOTOR, HIGH);
        vTaskDelay(300/portTICK_PERIOD_MS);
        digitalWrite(VIBRATION_MOTOR, LOW);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

static void _loop(void *params) { while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    beep(tune);
    xTaskNotifyGive(eda_listener_trigger);
}}
