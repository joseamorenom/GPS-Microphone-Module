#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_ID uart0 ///<Configuración de UART que se usará
#define BAUD_RATE 115200  ///<Velocidad de baudios predeterminada del NEO-6M
#define UART_TX_PIN 12///<Asignación del GPIO para tranmisión
#define UART_RX_PIN 13///<Asignación del GPIO para Recepción 
#define PPS_PIN 2
 

bool gps_init() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    return uart_is_enabled(UART_ID);
} 

/**
 * @brief Convierte una coordenada en formato grados y minutos a formato decimal.
 * @param coord La coordenada en formato grados y minutos.
 * @param direction La dirección (N, S, E, W).
 * @return La coordenada en formato decimal.
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

    ///<Ajustar para hemisferios sur y oeste
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }

    return decimal;
}

/**
 * @brief Analiza una sentencia GGA del GPS.
 * @param sentence La sentencia GGA a analizar según el protocolo NMEA
 */

void parse_gga_sentence(const char* sentence) {
    char time[10], lat[15], ns, lon[15], ew;
    int fix_quality;

    sscanf(sentence, "$GPGGA,%9[^,],%14[^,],%c,%14[^,],%c,%d", 
        time, lat, &ns, lon, &ew, &fix_quality);

    if (fix_quality > 0) {
        double latitude = convert_to_decimal(lat, ns);
        double longitude = convert_to_decimal(lon, ew);
        printf("Time: %s, Latitude: %f %c, Longitude: %f %c\n", time, latitude, ns, longitude, ew);
        
        ///<Construir URL para Google Maps
        char url[100];
        snprintf(url, sizeof(url), "https://www.google.com/maps?q=%f,%f", latitude, longitude);
        printf("Google Maps URL: %s\n", url);
    } else {
        printf("No GPS fix.\n");
    }
}

/**
 * @brief Lee los datos del GPS.
*/
void read_gps_data() {
    char buffer[256]; ///<Se declara el tamaño del buffer
    int index = 0;

    while (true) {
        printf("Leyendo UART\n");
        if (uart_is_readable(UART_ID)) {
            printf("UART listo para leer\n");
            char c = uart_getc(UART_ID);
            if (c == '\n' || index >= sizeof(buffer) - 1) {
                buffer[index] = '\0';
                // printf("GPS Data: %s\n", buffer);  ///<Imprime los datos crudos

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

/**
 * @brief Función principal del programa.
 * @return 0 si el programa se ejecuta correctamente, 1 en caso de error.
*/

int main() {
    stdio_init_all();
    if (!gps_init()) {
        printf("Error: No se pudo inicializar el GPS.\n");
        
    }
    printf("GPS module initialized. Reading data...\n");   
    read_gps_data();
        
    

    return 0;
}