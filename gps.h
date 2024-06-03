/**
 * @file gps.h
 * @brief Header file for GPS module.
 *
 * This file contains the function declarations for initializing
 * and reading data from the GPS module.
 */

#ifndef GPS_H
#define GPS_H

/**
 * @brief Initializes the GPS module.
 *
 * This function sets up the GPS hardware, preparing it for use.
 */
void gps_init(void);

/**
 * @brief Reads GPS data.
 *
 * @param[out] latitude Pointer to a buffer to store the latitude.
 * @param[out] longitude Pointer to a buffer to store the longitude.
 * @return 1 if GPS signal is available, 0 otherwise.
 */
int gps_read(char *latitude, char *longitude);

#endif // GPS_H
