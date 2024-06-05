#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UART_ID uart1
#define BAUD_RATE 9600
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define PPS_PIN 9

bool gps_init() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    printf("UART initialized\n");
    return uart_is_enabled(UART_ID);
}

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
