// Librería para el manejo del GPS

//  */

#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool gps_init();
double convert_to_decimal(const char* coord, char direction);
void parse_gga_sentence(const char* sentence);
void read_gps_data();

#endif