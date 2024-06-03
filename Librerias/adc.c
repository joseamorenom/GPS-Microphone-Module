/**
 * @file adc.c
 * @brief Implementation file for ADC module.
 *
 * This file contains the function definitions for initializing
 * and reading from the ADC module.
 */

#include "adc.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

/**
 * @brief Initializes the ADC module.
 */
void adc_init(void) {
    adc_init();
    adc_gpio_init(26); // Pin GP26 (ADC0)
    adc_select_input(0); // ADC Channel 0
}

/**
 * @brief Reads a value from the ADC.
 *
 * @return The ADC value as a 16-bit integer.
 */
uint16_t adc_read(void) {
    return adc_read();
}
