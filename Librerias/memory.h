/**
 * @file memory.h
 * @brief Header file for memory module.
 *
 * This file contains the function declarations for initializing
 * and writing data to non-volatile memory.
 */

#ifndef MEMORY_H
#define MEMORY_H

/**
 * @brief Initializes the memory module.
 *
 * This function sets up the memory hardware, preparing it for use.
 */
void memory_init(void);

/**
 * @brief Writes data to memory.
 *
 * @param[in] data Pointer to the data to be written.
 */
void memory_write(const char *data);

#endif // MEMORY_H
