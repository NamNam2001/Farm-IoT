#include "led.h"

void led_init()
{
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
    for (int i = 0; i < sizeof(LED_NOTIFICATION) / sizeof(int); i++)
    {
        pinMode(LED_NOTIFICATION[i], OUTPUT);
        digitalWrite(LED_NOTIFICATION[i], LOW);
    }
}

void led_on(uint8_t hex_led)
{
    for (int i = 0; i < sizeof(LED_NOTIFICATION) / sizeof(int); i++)
    {
        if (hex_led & (1 << i))
        {
            digitalWrite(LED_NOTIFICATION[i], HIGH);
        }
        else
        {
            digitalWrite(LED_NOTIFICATION[i], LOW);
        }
    }
}

void led_off(uint8_t hex_led)
{
    for (int i = 0; i < sizeof(LED_NOTIFICATION) / sizeof(int); i++)
    {
        if (hex_led & (1 << i))
        {
            digitalWrite(LED_NOTIFICATION[i], LOW);
        }
    }
}