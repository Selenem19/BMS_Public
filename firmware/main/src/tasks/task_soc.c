/**
 * @file task_soc.c
 * @brief Coulomb Counting algorithm for SoC (with EMA filter) and thermal derating for SoP (with hysteresis).
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bms_types.h"
#include "esp_log.h"
#include <stdbool.h> // Required for boolean variables (true/false)

static const char *TAG = "STATE_ESTIMATION";

// --- BATTERY PHYSICAL CONFIGURATION ---
#define NOMINAL_CAPACITY_AH  40.0f
#define TASK_PERIOD_MS       100

// --- FILTER PARAMETERS ---
#define ALPHA_CURRENT        0.2f  // Smoothing factor (0.0 = max filter, 1.0 = no filter)

void Task_BMS_StateEstimation(void *pvParameters) {
    (void)pvParameters;

    ESP_LOGI(TAG, "Starting SoC and SoP estimators...");

    bms_telemetry.soc = 100.0f;
    bms_telemetry.sop = 100.0f;
    bms_telemetry.soh = 100.0f; 

    float capacity_as = NOMINAL_CAPACITY_AH * 3600.0f;
    float delta_t = (float)TASK_PERIOD_MS / 1000.0f;

    // --- PERSISTENT VARIABLES (Memory between loop cycles) ---
    float filtered_current = 0.0f;
    bool is_first_cycle = true;
    bool emergency_lock = false; // Memory flag for State 4 (Critical Failure Hysteresis)

    while (1) {
        // --- 1. SIGNAL CONDITIONING & SoC CALCULATION ---
        float raw_current = bms_raw_data.pack_current;
        
        // Initialize the filter on the first cycle to avoid a steep ramp from 0
        if (is_first_cycle) {
            filtered_current = raw_current;
            is_first_cycle = false;
        } else {
            // Apply Digital Low-Pass Filter (EMA)
            filtered_current = (ALPHA_CURRENT * raw_current) + ((1.0f - ALPHA_CURRENT) * filtered_current);
        }

        // Coulomb Counting integration using the cleaned signal
        float consumed_coulombs = filtered_current * delta_t;
        float soc_variation = (consumed_coulombs / capacity_as) * 100.0f;

        bms_telemetry.soc -= soc_variation;

        // Safety Saturation Limits [0% - 100%]
        if (bms_telemetry.soc > 100.0f) bms_telemetry.soc = 100.0f;
        if (bms_telemetry.soc < 0.0f)   bms_telemetry.soc = 0.0f;


        // --- 2. SoP CALCULATION (THERMAL DERATING & HYSTERESIS) ---
        // Find the highest temperature in the pack
        float t_max = 0.0f;
        for(int j = 0; j < TOTAL_TEMPS; j++) {
            if(bms_raw_data.temperatures[j] > t_max) t_max = bms_raw_data.temperatures[j];
        }

        // Evaluate latching conditions (Emergency Hysteresis)
        if (t_max >= 60.0f) {
            emergency_lock = true;  // Hits 60ºC: Lock traction
        } else if (emergency_lock && t_max <= 50.0f) {
            emergency_lock = false; // Only release when cooled down to 50ºC
        }

        // Apply SoP logic based on the latching state
        if (emergency_lock) {
            bms_telemetry.sop = 0.0f;   // State 4: Emergency, zero traction
        } 
        else {
            // States 1, 2, and 3 (Below 60ºC and no active lock)
            if (t_max < 50.0f) {
                bms_telemetry.sop = 100.0f; // Nominal state: full power available
            } else {
                // State 3: Linear interpolation between 50ºC (100%) and 60ºC (0%)
                bms_telemetry.sop = 100.0f - ((t_max - 50.0f) * 10.0f);
            }
        }


        // --- 3. EXTREME VALUES MAPPING FOR CAN ---
        // Output the filtered current to the CAN bus for telemetry stability
        bms_telemetry.current = filtered_current; 
        
        float v_max = 0.0f;
        float v_min = 5.0f;
        for(int i = 0; i < TOTAL_CELLS; i++) {
            if(bms_raw_data.cell_voltages[i] > v_max) v_max = bms_raw_data.cell_voltages[i];
            if(bms_raw_data.cell_voltages[i] < v_min) v_min = bms_raw_data.cell_voltages[i];
        }
        bms_telemetry.max_voltage = v_max;
        bms_telemetry.min_voltage = v_min;

        // --- 4. TELEMETRY OUTPUT ---
        ESP_LOGI(TAG, "SoC: %.4f %% | SoP: %3.0f %% | I_filt: %4.1fA | Max T: %.1f C | Lock: %d", 
                 bms_telemetry.soc, bms_telemetry.sop, filtered_current, t_max, emergency_lock);

        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS));
    }
}