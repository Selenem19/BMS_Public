/**
 * @file task_safety.c
 * @brief Safety monitoring task that supervises operating limits and disconnects the contactors in case of a fault.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bms_types.h"
#include "bms_hardware.h" 

static const char *TAG = "SAFETY_MONITOR";

// --- SAFETY LIMITS (HARD LIMITS) ---
#define MAX_CELL_VOLTAGE 4.20f
#define MIN_CELL_VOLTAGE 3.00f
#define MAX_TEMPERATURE  60.0f

void Task_SafetyMonitor(void *pvParameters) {
    ESP_LOGW(TAG, "Iniciando Perro Guardian... Esperando estabilizacion de sensores (1s)");

    // Allow the system a short startup delay so that the sensors (or the simulator)
    // can provide valid measurements before the first safety check.
    // This prevents false undervoltage detections caused by initial 0.0 V readings.
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        bool fault_detected = false;

        // 1. Check all battery cells for voltage violations
        for (int i = 0; i < TOTAL_CELLS; i++) {
            if (bms_raw_data.cell_voltages[i] > MAX_CELL_VOLTAGE) {
                ESP_LOGE(TAG, "¡PELIGRO! Sobretension en Celda %d: %.3f V", i, bms_raw_data.cell_voltages[i]);
                fault_detected = true;
            }
            if (bms_raw_data.cell_voltages[i] < MIN_CELL_VOLTAGE) {
                ESP_LOGE(TAG, "¡PELIGRO! Subtension en Celda %d: %.3f V", i, bms_raw_data.cell_voltages[i]);
                fault_detected = true;
            }
        }

         // 2. Check all temperature sensors
        for (int j = 0; j < TOTAL_TEMPS; j++) {
            if (bms_raw_data.temperatures[j] > MAX_TEMPERATURE) {
                ESP_LOGE(TAG, "¡PELIGRO! Sobretemperatura en Sensor %d: %.1f C", j, bms_raw_data.temperatures[j]);
                fault_detected = true;
            }
        }

       // 3. If a fault is detected, execute the emergency shutdown procedure
        if (fault_detected) {
            ESP_LOGE(TAG, "¡FALLO CRITICO DETECTADO! ACTIVANDO BANDERA DE EMERGENCIA...");
            
            // Raise the global emergency flag so that all tasks are notified
            bms_emergency_fault = true; 

            // Immediately disconnect the battery pack for safety
            BMS_Set_MainContactor(false);
            BMS_Set_PrechargeRelay(false);

            while(1) {
                ESP_LOGE(TAG, "SISTEMA BLOQUEADO POR SEGURIDAD. REINICIE EL BMS.");
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        }
        
        // 4. Safety monitoring period: check the sensors every 50 ms (20 Hz)
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}