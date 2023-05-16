#include <Arduino.h>

#ifndef EDA_PIN
#define EDA_PIN

#define VIBRATION_MOTOR 2
#define BUZZER 4
#define TILT_SENSOR 12

#define SHIFT_CLK 27 // Clock
#define SHIFT_KLK 13 // Klock
#define SHIFT_DATA 14

#define OLED_CS 5
#define OLED_SCK 18
#define OLED_SDA 23
#define OLED_RESET 21
#define OLED_RS 22
#define OLED_LIGHT 26

#define TILT_ACTIVE HIGH

static void eda_pin_init() {
    pinMode(VIBRATION_MOTOR, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(TILT_SENSOR, INPUT_PULLUP);

    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_KLK, OUTPUT);
    pinMode(SHIFT_DATA, OUTPUT);

    pinMode(OLED_CS, OUTPUT);
    pinMode(OLED_SCK, OUTPUT);
    pinMode(OLED_SDA, OUTPUT);
    pinMode(OLED_RESET, OUTPUT);
    pinMode(OLED_RS, OUTPUT);
    pinMode(OLED_LIGHT, OUTPUT);
}

#endif
