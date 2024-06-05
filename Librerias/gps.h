/**
 * @file gps.h
 * @brief Librería para el manejo del GPS en Raspberry Pi Pico
 * 
 * Este archivo contiene las declaraciones de las funciones necesarias para inicializar,
 * leer y procesar datos de un módulo GPS conectado a una Raspberry Pi Pico.
 */

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

/**
 * @brief Inicializa el módulo GPS configurando el UART
 * 
 * Esta función configura el UART para comunicarse con el módulo GPS y 
 * configura los pines GPIO correspondientes.
 * 
 * @return true si el UART se inicializa correctamente, false en caso contrario
 */

bool gps_init();

/**
 * @brief Convierte coordenadas en formato NMEA a formato decimal
 * 
 * Esta función convierte una coordenada en formato NMEA (grados y minutos)
 * a formato decimal (grados).
 * 
 * @param coord La coordenada en formato NMEA (cadena de caracteres)
 * @param direction El hemisferio ('N', 'S', 'E', 'W')
 * @return La coordenada en formato decimal
 */

double convert_to_decimal(const char* coord, char direction);

/**
 * @brief Analiza una sentencia GGA del GPS
 * 
 * Esta función analiza una sentencia GGA recibida del GPS, extrae los datos relevantes
 * y los imprime en la consola. También convierte las coordenadas a formato decimal
 * y genera una URL de Google Maps.
 * 
 * @param sentence La sentencia GGA completa como cadena de caracteres
 */

void parse_gga_sentence(const char* sentence);

/**
 * @brief Lee datos del GPS a través del UART
 * 
 * Esta función lee continuamente datos del GPS a través del UART, 
 * analiza las sentencias GGA y procesa los datos de localización.
 */

void read_gps_data();

#endif
