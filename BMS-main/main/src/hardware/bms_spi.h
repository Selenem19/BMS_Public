/**
 * @file bms_spi.h
 * @brief SPI bus interface for communication with the LTC6811 slave devices.
 */

#ifndef BMS_SPI_H
#define BMS_SPI_H

#include <stdint.h>

// Hardware interface function prototypes
void BMS_SPI_Init(void);
void LTC6811_Send_Command(uint8_t cmd_byte1, uint8_t cmd_byte2);

#endif // BMS_SPI_H