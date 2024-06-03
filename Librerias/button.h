/**
 * @file button.h
 * @brief Header file for button control module.
 *
 * This file contains the function declarations for initializing
 * and reading the state of a button.
 */

#ifndef BUTTON_H
#define BUTTON_H

/**
 * @brief Initializes the button module.
 *
 * This function sets up the GPIO pin for the button.
 */
void button_init(void);

/**
 * @brief Checks if the button is pressed.
 *
 * @return 1 if the button is pressed, 0 otherwise.
 */
int button_is_pressed(void);

#endif // BUTTON_H
