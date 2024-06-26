/**
 * @file main.c
 * @brief Programa principal para el control del flujo
 *
 * This file contains the main function which initializes all modules,
 * handles the main loop, and manages the measurement of noise levels
 * and geolocation.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "adc.h"
#include "gps.h"
#include "memory.h"
#include "led.h"
#include "button.h"


#define LED_GREEN 2 //Se activa cuando el dispositivo se enciende y cuando 
#define LED_YELLOW 3 //Se activa cuando se oprime el pulsador y hay conexion
#define LED_ORANGE 4 //Parpadea al guardar la información
#define LED_RED 5 // Se enciende por tres segundos si se unde el pulsador cuando se esta grabando
                  // Parpadea si se pierde el GPS
#define BUTTON_PIN 15

/**
 * @brief Mide el ruido y lo guarda con la geolocalizacion
 *
 * This function reads the noise level using the ADC, obtains the GPS
 * coordinates, and stores the data in non-volatile memory. It also
 * manages the LED indicators to show the current state of the system.
 */
 
void measure_noise_level(void);

int main() {
    stdio_init_all();
    adc_init();
    if (!gps_init()) {
        printf("Error: No se pudo inicializar el GPS.\n");
        return 1;
    }
    printf("GPS module initialized. Reading data...\n");
    memory_init();
    led_init();
    button_init();

    led_set_state(LED_GREEN, 1); // Ready state

    while (true) {
        if (button_is_pressed()) {
            measure_noise_level();
        }
        sleep_ms(100);
    }

    return 0;
}

void measure_noise_level() {
    // Indicate start of measurement
    led_set_state(LED_GREEN, 0);
    led_set_state(LED_YELLOW, 1);

    uint16_t noise_level = 0;
    for (int i = 0; i < 10; i++) {
        if (button_is_pressed()) {
            // Button pressed during measurement, abort and turn on red LED
            led_set_state(LED_YELLOW, 0);
            led_set_state(LED_RED, 1);
            sleep_ms(3000);
            led_set_state(LED_RED, 0);
            led_set_state(LED_GREEN, 1);
            return;
        }
        noise_level += adc_read();
        sleep_ms(1000); // Sample every second for 10 seconds
    }
    noise_level /= 10; // Average value

    // Get GPS data
    char buffer[256];
    bool gps_fix = false;
    double latitude, longitude;

    while (!gps_fix) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            static int index = 0;

            if (c == '\n' || index >= sizeof(buffer) - 1) {
                buffer[index] = '\0';

                if (strstr(buffer, "$GPGGA") != NULL) {
                    char time[10], lat[15], ns, lon[15], ew;
                    int fix_quality;
                    sscanf(buffer, "$GPGGA,%9[^,],%14[^,],%c,%14[^,],%c,%d", 
                        time, lat, &ns, lon, &ew, &fix_quality);

                    if (fix_quality > 0) {
                        latitude = convert_to_decimal(lat, ns);
                        longitude = convert_to_decimal(lon, ew);
                        gps_fix = true;
                    }
                }

                index = 0;
            } else {
                buffer[index++] = c;
            }
        }

        if (button_is_pressed()) {
            // Button pressed during GPS data reading, abort and turn on red LED
            led_set_state(LED_YELLOW, 0);
            led_set_state(LED_RED, 1);
            sleep_ms(3000);
            led_set_state(LED_RED, 0);
            led_set_state(LED_GREEN, 1);
            return;
        }
    }

    // Store data
    char data[64];
    snprintf(data, sizeof(data), "Noise: %u, Lat: %f, Lon: %f\n", noise_level, latitude, longitude);
    memory_write(data);

    // Indicate end of measurement
    led_set_state(LED_YELLOW, 0);
    led_set_state(LED_ORANGE, 1);
    sleep_ms(500);
    led_set_state(LED_ORANGE, 0);
    led_set_state(LED_GREEN, 1);
}