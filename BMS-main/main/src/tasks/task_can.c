/**
 * @file task_can.c
 * @brief Telemetry transmission task via CAN bus (TWAI).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "bms_types.h"
#include <string.h>

static const char *TAG = "CAN_BUS";

// --- CAN HARDWARE CONFIGURATION ---
// Standard CAN pins on ESP32 development boards (adjust if using different ones on your PCB)
#define TX_GPIO_NUM 21
#define RX_GPIO_NUM 22

void Task_BMS_CAN(void *pvParameters) {
    (void)pvParameters;

    ESP_LOGI(TAG, "Iniciando modulo de comunicaciones CAN (TWAI)...");

    // 1. Driver Configuration
    // We use NO_ACK to be able to simulate without having a real inverter connected to the other side of the cable.
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS(); // Automotive standard
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // 2. Installation and Startup
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "Driver TWAI instalado correctamente.");
    } else {
        ESP_LOGE(TAG, "Fallo critico al instalar el driver TWAI.");
        vTaskDelete(NULL);
    }

    if (twai_start() == ESP_OK) {
        ESP_LOGI(TAG, "CAN Bus ONLINE. Velocidad: 500 kbps.");
    }

    // 3. Main Transmission Loop (10 Hz)
    while (1) {
        // Get the maximum pack temperature to send it
        float max_t = 0.0f;
        for(int j = 0; j < TOTAL_TEMPS; j++) {
            if(bms_raw_data.temperatures[j] > max_t) max_t = bms_raw_data.temperatures[j];
        }

        // --- FRAME 0x100: ENERGY MANAGEMENT ---
        twai_message_t msg_energy = {
            .identifier = 0x100,
            .data_length_code = 8,
            .extd = 0 // We use standard 11-bit ID
        };
        // Copy the 4 bytes of the SoC float into the first 4 bytes of the message
        memcpy(&msg_energy.data[0], &bms_telemetry.soc, 4);
        // Copy the 4 bytes of the SoP float into the last 4 bytes of the message
        memcpy(&msg_energy.data[4], &bms_telemetry.sop, 4);

        // --- FRAME 0x101: PHYSICAL SAFETY ---
        twai_message_t msg_safety = {
            .identifier = 0x101,
            .data_length_code = 8,
            .extd = 0
        };
        memcpy(&msg_safety.data[0], &max_t, 4);
        memcpy(&msg_safety.data[4], &bms_raw_data.pack_current, 4);

        // Send frames to the network
        if (twai_transmit(&msg_energy, pdMS_TO_TICKS(10)) != ESP_OK || 
            twai_transmit(&msg_safety, pdMS_TO_TICKS(10)) != ESP_OK) {
            // ESP_LOGW(TAG, "Cola TX CAN llena. Saturacion de bus.");
        } else {
            // Uncomment the following line to debug what is being sent, 
            // although at 10Hz it will flood the console.
            // ESP_LOGD(TAG, "Tramas 0x100 y 0x101 enviadas al Inversor.");
        }
        
        twai_status_info_t status;
        twai_get_status_info(&status);

        ESP_LOGI(TAG, "TX pending: %d", status.msgs_to_tx);
        
        // Automotive CAN Bus usually broadcasts this data at 10Hz (every 100ms)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}