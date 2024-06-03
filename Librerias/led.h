/**
 * @file led.h
 * @brief Header file for LED control module.
 *
 * This file contains the function declarations for initializing
 * and controlling the state of LEDs.
 */

#ifndef LED_H
#define LED_H

/**
 * @brief Initializes the LED module.
 *
 * This function sets up the GPIO pins for the LEDs.
 */
void led_init(void);

/**
 * @brief Sets the state of an LED.
 *
 * @param[in] led The LED to control.
 * @param[in] state The state to set (0 for off, 1 for on).
 */
void led_set_state(int led, int state);

#endif // LED_H
