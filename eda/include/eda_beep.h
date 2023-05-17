/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     */

#include "eda_pin_definitions.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#ifndef EDA_BEEP
#define EDA_BEEP

void eda_beep_init();
void eda_beep_setTuneIndex(int);
void eda_beep_trigger();

extern TaskHandle_t eda_listener_trigger;

#endif
