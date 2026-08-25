/**
 * @file task_simulator.c
 * @brief Simulation of voltage and temperature measurements for a 30-cell battery pack.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bms_types.h"
#include <stdlib.h> 

static const char *TAG = "SIMULATOR";

BMS_Raw_Data_t bms_raw_data;

void Task_BMS_Simulator(void *pvParameters) {
    (void)pvParameters; 

    ESP_LOGI(TAG, "Iniciando Simulador del Pack de Baterias (30 Celdas)...");

    bms_raw_data.is_simulating = true;
    bms_raw_data.pack_current = 0.0f; 

    for(int i = 0; i < TOTAL_CELLS; i++) {
        bms_raw_data.cell_voltages[i] = 3.85f; 
    }

    for(int j = 0; j < TOTAL_TEMPS; j++) {
        bms_raw_data.temperatures[j] = 25.0f;
    }

    while (1) {
        float sum_voltage = 0.0f;
        float current_noise = 0.0f;

        // --- CURRENT AND TEMPERATURE DYNAMICS ---
        if (bms_emergency_fault == true) {
            bms_raw_data.pack_current = 0.0f; 

            for(int j = 0; j < TOTAL_TEMPS; j++) {
                if (bms_raw_data.temperatures[j] > 25.0f) {
                    bms_raw_data.temperatures[j] -= 0.6f; 
                }
            }
                } else {
            // --- 1. THE INVERTER FOLLOWS THE SoP ---
            // Read the limit requested by the BMS (if the BMS requests 90%, limit to 90%)
            float current_sop = bms_telemetry.sop;
            float driver_demand = 15.0f; // The driver requests 15 A at full throttle
            float max_current = driver_demand * (current_sop / 100.0f);

            current_noise = ((float)rand() / RAND_MAX) * 1.0f - 0.5f;
            bms_raw_data.pack_current = (max_current > 0.0f) ? max_current + current_noise : 0.0f;

            // --- 2. THERMAL MODEL (Heating vs Cooling) ---
            // At 15 A, around 0.5 °C is generated per cycle (I² * 0.0022)
            float generated_heat = (bms_raw_data.pack_current * bms_raw_data.pack_current) * 0.0022f;

            // The fans remove heat depending on the temperature
            float removed_heat = 0.0f;
            float current_temp = bms_raw_data.temperatures[0];

            if (current_temp >= 50.0f) {
                removed_heat = 0.4f; // Fan at 100% (removes 0.4 °C)
            } else if (current_temp > 35.0f) {
                // Fan speed increases proportionally
                removed_heat = 0.1f + ((current_temp - 35.0f) / 15.0f) * 0.3f;
            }

            // Apply the thermal balance to all temperature sensors
            for(int j = 0; j < TOTAL_TEMPS; j++) {
                bms_raw_data.temperatures[j] += (generated_heat - removed_heat);

                if (bms_raw_data.temperatures[j] < 25.0f) {
                    bms_raw_data.temperatures[j] = 25.0f; // Ambient temperature limit
                }
            }
        }
        // --- 3. CELL VOLTAGE DYNAMICS ---
        for(int i = 0; i < TOTAL_CELLS; i++) {
            float noise = ((float)rand() / RAND_MAX) * 0.01f - 0.005f;
            float voltage_drop = (bms_raw_data.pack_current * 0.0001f);
            bms_raw_data.cell_voltages[i] = bms_raw_data.cell_voltages[i] - voltage_drop + noise;
            
            if (xTaskGetTickCount() > pdMS_TO_TICKS(60000)) {
                if (i == 7) bms_raw_data.cell_voltages[i] = 4.30f; 
            }
            sum_voltage += bms_raw_data.cell_voltages[i];
        }

        bms_raw_data.pack_voltage = sum_voltage;

        // Print the current simulated telemetry
        ESP_LOGI(TAG, "Pack Volts: %.2f V | Current: %5.2f A | Temp Max: %.1f C", 
                 bms_raw_data.pack_voltage, 
                 bms_raw_data.pack_current, 
                 bms_raw_data.temperatures[0]);

        vTaskDelay(pdMS_TO_TICKS(250)); 
    }
}