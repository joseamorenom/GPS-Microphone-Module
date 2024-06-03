/**
 * @file led.c
 * @brief Implementation file for LED control module.
 *
 * This file contains the function definitions for initializing
 * and controlling the state of LEDs.
 */

#include "led.h"
#include "pico/stdlib.h"

#define LED_GREEN 2
#define LED_YELLOW 3
#define LED_ORANGE 4
#define LED_RED 5

/**
 * @brief Initializes the LED module.
 */
void led_init(void) {
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_init(LED_ORANGE);
    gpio_set_dir(LED_ORANGE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
}

/**
 * @brief Sets the state of an LED.
 *
 * @param[in] led The LED to control.
 * @param[in] state The state to set (0 for off, 1 for on).
 */
void led_set_state(int led, int state) {
    gpio_put(led, state);
}
