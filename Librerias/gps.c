/**
 * @file main.c
 * @brief Ejemplo de inicialización y lectura de datos GPS usando UART en Raspberry Pi Pico
 * 
 * Este programa inicializa un módulo GPS a través de UART y lee los datos NMEA.
 * Analiza las sentencias GGA para extraer información de localización y la imprime en la consola.
 * 
 */

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_ID uart1  ///< ID del UART utilizado para la comunicación GPS
#define BAUD_RATE 9600 ///< Tasa de baudios para la comunicación UART
#define UART_TX_PIN 4  ///< Pin GPIO utilizado para la transmisión UART
#define UART_RX_PIN 5  ///< Pin GPIO utilizado para la recepción UART
#define PPS_PIN 9      ///< Pin GPIO utilizado para el pulso por segundo (PPS)

/**
 * @brief Inicializa el módulo GPS configurando el UART
 * 
 * Esta función configura el UART para comunicarse con el módulo GPS y 
 * configura los pines GPIO correspondientes.
 * 
 * @return true si el UART se inicializa correctamente, false en caso contrario
 */

bool gps_init() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    printf("UART initialized\n");
    return uart_is_enabled(UART_ID);
}

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

double convert_to_decimal(const char* coord, char direction) {
    double degrees = 0;
    double minutes = 0;

    if (direction == 'N' || direction == 'S') {
        degrees = atof((char[]) { coord[0], coord[1], '\0' });
        minutes = atof(coord + 2);
    } else if (direction == 'E' || direction == 'W') {
        degrees = atof((char[]) { coord[0], coord[1], coord[2], '\0' });
        minutes = atof(coord + 3);
    }

    double decimal = degrees + (minutes / 60.0);

    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }

    return decimal;
}

/**
 * @brief Analiza una sentencia GGA del GPS
 * 
 * Esta función analiza una sentencia GGA recibida del GPS, extrae los datos relevantes
 * y los imprime en la consola. También convierte las coordenadas a formato decimal
 * y genera una URL de Google Maps.
 * 
 * @param sentence La sentencia GGA completa como cadena de caracteres
 */

void parse_gga_sentence(const char* sentence) {
    char time[10], lat[15], ns, lon[15], ew;
    int fix_quality, num_satellites;
    float hdop, altitude, geoid_sep;

    sscanf(sentence, "$GPGGA,%9[^,],%14[^,],%c,%14[^,],%c,%d,%d,%f,%f,M,%f,M", 
        time, lat, &ns, lon, &ew, &fix_quality, &num_satellites, &hdop, &altitude, &geoid_sep);

    printf("Parsed GGA Sentence: time: %s, lat: %s, ns: %c, lon: %s, ew: %c, fix_quality: %d, num_satellites: %d, hdop: %.2f, altitude: %.2f, geoid_sep: %.2f\n", 
        time, lat, ns, lon, ew, fix_quality, num_satellites, hdop, altitude, geoid_sep);

    if (fix_quality > 0) {
        double latitude = convert_to_decimal(lat, ns);
        double longitude = convert_to_decimal(lon, ew);
        printf("Time: %s, Latitude: %f %c, Longitude: %f %c\n", time, latitude, ns, longitude, ew);
        
        char url[100];
        snprintf(url, sizeof(url), "https://www.google.com/maps?q=%f,%f", latitude, longitude);
        printf("Google Maps URL: %s\n", url);
    } else {
        printf("No GPS fix. fix_quality: %d\n", fix_quality);
    }
}

/**
 * @brief Lee datos del GPS a través del UART
 * 
 * Esta función lee continuamente datos del GPS a través del UART, 
 * analiza las sentencias GGA y procesa los datos de localización.
 */

void read_gps_data() {
    char buffer[256];
    int index = 0;

    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '\n' || index >= sizeof(buffer) - 1) {
                buffer[index] = '\0';
                printf("GPS Data: %s\n", buffer);

                if (strstr(buffer, "$GPGGA") != NULL) {
                    parse_gga_sentence(buffer);
                }

                index = 0;
            } else {
                buffer[index++] = c;
            }
        }
    }
}


/*
EJEMPLO DE CÓMO IMPLEMENTAR 
int main() {
    stdio_init_all();
    if (!gps_init()) {
        printf("Error: No se pudo inicializar el GPS.\n");
    }
    printf("GPS module initialized. Reading data...\n");
    read_gps_data();

    return 0;
}

*/
