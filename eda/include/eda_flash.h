#include "eda_pin_definitions.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>

#ifndef EDA_FLASH
#define EDA_FLASH

void eda_flash_init(int ticks = 10, int pulse_width = 6);
void eda_flash_trigger();
void eda_flash_allOn();
void eda_flash_allOff();

#endif
