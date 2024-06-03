/**
 * @file button.c
 * @brief Implementation file for button control module.
 *
 * This file contains the function definitions for initializing
 * and reading the state of a button.
 */

#include "button.h"
#include "pico/stdlib.h"

#define BUTTON_PIN 15

/**
 * @brief Initializes the button module.
 */
void button_init(void) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

/**
 * @brief Checks if the button is pressed.
 *
 * @return 1 if the button is pressed, 0 otherwise.
 */
int button_is_pressed(void) {
    return !gpio_get(BUTTON_PIN);
}
