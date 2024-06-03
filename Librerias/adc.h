/**
 * @file adc.h
 * @brief Header file for ADC module.
 *
 * This file contains the function declarations for initializing
 * and reading from the ADC module.
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/**
 * @brief Initializes the ADC module.
 *
 * This function sets up the ADC hardware, preparing it for use.
 */
void adc_init(void);

/**
 * @brief Reads a value from the ADC.
 *
 * @return The ADC value as a 16-bit integer.
 */
uint16_t adc_read(void);

#endif // ADC_H
