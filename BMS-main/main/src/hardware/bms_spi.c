/**
 * @file bms_spi.c
 * @brief Low-level SPI driver for ESP32 SPI bus initialization.
 */

#include "bms_spi.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "BMS_SPI";

// ESP32 pin assignments for the isoSPI interface
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

spi_device_handle_t spi_bms_handle;

void BMS_SPI_Init(void) {
    ESP_LOGI(TAG, "Inicializando bus SPI para LTC6811...");

    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 128
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000, // 1 MHz
        .mode = 3,                 // SPI Mode 3 required by the LTC6811
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7
    };

     // Initialize the physical SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    // Register the LTC6811 device on the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_bms_handle));
    
    ESP_LOGI(TAG, "Bus SPI inicializado correctamente.");
}

void LTC6811_Send_Command(uint8_t cmd_byte1, uint8_t cmd_byte2) {
    // Example command frame including placeholder PEC15 (CRC) bytes
    uint8_t tx_data[4] = {cmd_byte1, cmd_byte2, 0x00, 0x00}; 

    spi_transaction_t t = {
        .length = 8 * 4,
        .tx_buffer = tx_data,
        .rx_buffer = NULL
    };

    spi_device_polling_transmit(spi_bms_handle, &t);
}