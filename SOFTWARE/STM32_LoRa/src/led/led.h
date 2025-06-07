#ifndef LED_H
#define LED_H
#include <Arduino.h>
void led_init();
void led_on(uint8_t hex_led);
void led_off(uint8_t hex_led);
constexpr int LED_NOTIFICATION[4]{
    PB6,
    PB5,
    PB4,
    PB3,
};
#endif