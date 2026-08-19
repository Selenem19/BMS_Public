/**
 * @file task_balancing.c
 * @brief Control of the Buck-Boost system for intra-slave active balancing (Proportional Control).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bms_types.h"

static const char *TAG = "BALANCING";

// --- HARDWARE DESIGN PARAMETERS ---
#define IMBALANCE_THRESHOLD_V     0.020f 
#define BALANCING_CURRENT_MIN_A   0.5f    // 500mA specified as minimum
#define BALANCING_CURRENT_MAX_A   2.0f    // Thermal/physical safety limit of the circuit
#define MAX_PCB_TEMP_C            75.0f   // Maximum safe operating temperature for the slave PCB

void Task_BMS_Balancing(void *pvParameters) {
    (void)pvParameters;

    ESP_LOGI(TAG, "Iniciando Balanceo Activo Modular (3 Esclavos en paralelo)...");
    ESP_LOGI(TAG, "Hardware Buck-Boost configurado -> Rango dinámico: %.1fA a %.1fA", 
             BALANCING_CURRENT_MIN_A, BALANCING_CURRENT_MAX_A);
    
    // Turn off the 3 converters at startup
    for(int slave = 0; slave < NUM_SLAVES; slave++) {
        bms_balancing_status[slave].active = false;
    }

    while (1) {
        // Analyze each slave independently
        for(int slave = 0; slave < NUM_SLAVES; slave++) {
            
            // ---------------------------------------------------------
            // 1. THERMAL SAFETY OVERRIDE
            // ---------------------------------------------------------
            // Note: Adapt 'slave_pcb_temperatures' to the exact name in your bms_types.h struct
            float current_pcb_temp = bms_raw_data.slave_pcb_temperatures[slave]; 
            
            if (current_pcb_temp > MAX_PCB_TEMP_C) {
                if (bms_balancing_status[slave].active == true) {
                    ESP_LOGE(TAG, "[ESCLAVO %d] SOBRECARGA TÉRMICA (%.1fC). ¡Desactivando balanceo activo para proteger el hardware!", slave, current_pcb_temp);
                }
                bms_balancing_status[slave].active = false;
                continue; // Skip the rest of the balancing logic for this slave until it cools down
            }

            // ---------------------------------------------------------
            // 2. PACK SCAN: Find donor and receiver
            // ---------------------------------------------------------
            float v_max = 0.0f;
            float v_min = 5.0f;
            uint8_t max_cell_id = 0;
            uint8_t min_cell_id = 0;

            // Cell range assigned to this slave
            int start_idx = slave * CELLS_PER_SLAVE;
            int end_idx = start_idx + CELLS_PER_SLAVE;

            for(int cell = start_idx; cell < end_idx; cell++) {
                if(bms_raw_data.cell_voltages[cell] > v_max) {
                    v_max = bms_raw_data.cell_voltages[cell];
                    max_cell_id = cell;
                }
                if(bms_raw_data.cell_voltages[cell] < v_min) {
                    v_min = bms_raw_data.cell_voltages[cell];
                    min_cell_id = cell;
                }
            }

            // ---------------------------------------------------------
            // 3. VOLTAGE DELTA EVALUATION
            // ---------------------------------------------------------
            float delta_v = v_max - v_min;
            bms_balancing_status[slave].delta_v = delta_v;

            // ---------------------------------------------------------
            // 4. ACTIVATION LOGIC (Proportional Control)
            // ---------------------------------------------------------
            if (delta_v > IMBALANCE_THRESHOLD_V && bms_emergency_fault == false) {
                bms_balancing_status[slave].active = true;
                bms_balancing_status[slave].source_cell = max_cell_id;
                bms_balancing_status[slave].target_cell = min_cell_id;
                
                // DYNAMIC CURRENT CALCULATION:
                // Base of 0.5A + an extra proportional to imbalance severity
                float dynamic_current = BALANCING_CURRENT_MIN_A + ((delta_v - IMBALANCE_THRESHOLD_V) * 10.0f);
                
                // Hardware safety saturation
                if (dynamic_current > BALANCING_CURRENT_MAX_A) {
                    dynamic_current = BALANCING_CURRENT_MAX_A;
                }

                ESP_LOGW(TAG, "[ESCLAVO %d] Trasvasando @ %.2fA : Celda %d (%.3fV) >>> Celda %d (%.3fV) | dV: %.3fV", 
                         slave, dynamic_current, max_cell_id, v_max, min_cell_id, v_min, delta_v);
            } else {
                if (bms_balancing_status[slave].active == true) {
                    ESP_LOGI(TAG, "[ESCLAVO %d] Celdas niveladas. Apagando conversor.", slave);
                }
                bms_balancing_status[slave].active = false;
            }
        }
        
        // Evaluate at 1 Hz
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}