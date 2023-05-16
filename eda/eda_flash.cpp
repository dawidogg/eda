/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * Module for flashing pink LEDs via a shift register (74HC595).                */

#include "include/eda_flash.h"

static int flash_ticks, flash_pulse_width;
static TaskHandle_t trigger = NULL;
static void _loop(void*);
static void all(bool);

///////////////////////////////////////////////////////////////////////////////

void eda_flash_init(int ticks, int pulse_width) {
    flash_ticks = ticks;
    flash_pulse_width = pulse_width;
    xTaskCreate(_loop, "FLASH", 1024, NULL, 1, &trigger);
}

void eda_flash_trigger() {
    xTaskNotifyGive(trigger);
}

static void pulse() {
    digitalWrite(SHIFT_CLK, !digitalRead(SHIFT_CLK));
    digitalWrite(SHIFT_KLK, !digitalRead(SHIFT_CLK));
}

static void _loop(void *params) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        digitalWrite(SHIFT_DATA, HIGH);
        for (int i = 0; i < flash_pulse_width; i++) {
            pulse();
            vTaskDelay(flash_ticks/portTICK_PERIOD_MS);
            pulse();
            vTaskDelay(flash_ticks/portTICK_PERIOD_MS);
        }

        digitalWrite(SHIFT_DATA, LOW);
        for (int i = 0; i < flash_pulse_width; i++) {
            pulse();
            vTaskDelay(flash_ticks/portTICK_PERIOD_MS);
            pulse();
            vTaskDelay(flash_ticks/portTICK_PERIOD_MS);
        }
    }
}


static void all(bool b) {
    digitalWrite(SHIFT_DATA, b);
    vTaskDelay(1/portTICK_PERIOD_MS);
    for (int i = 0; i < 6; i++) {
        pulse();
        vTaskDelay(1/portTICK_PERIOD_MS);
        pulse();
        vTaskDelay(1/portTICK_PERIOD_MS);
    }
    digitalWrite(SHIFT_DATA, !b);
}

void eda_flash_allOn() {
    all(HIGH);
}

void eda_flash_allOff() {
    all(LOW);
}

