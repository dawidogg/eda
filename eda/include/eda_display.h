/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     */

#include "eda_pin_definitions.h"
#include "eda_font.h"
#include "eda_font_encoder.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <freertos/FreeRTOS.h>

#ifndef EDA_DISPLAY
#define EDA_DISPLAY

void eda_display_init();
void eda_display_off();
void eda_display_clear();
void eda_display_print(String, bool _draw_lines = true);
void eda_display_introAnimation();

extern bool eda_stopFlag;

#endif
