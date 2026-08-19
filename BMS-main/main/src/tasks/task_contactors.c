#include "task_contactors.h"
#include "bms_hardware.h"
#include "bms_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "CONTACTOR_MGR";

void Task_ContactorManager(void *pvParameters) {
    ESP_LOGI(TAG, "Iniciando secuencia de precarga de alta tension...");
    
    BMS_Set_PrechargeRelay(true);
    
    // Smart loop: wait for 3 seconds while checking for emergency faults every 100 ms
    bool arranque_exitoso = true;
    for(int i = 0; i < 30; i++) {
        if (bms_emergency_fault == true) { 
            ESP_LOGE(TAG, "¡Emergencia durante la precarga! Abortando arranque...");
            arranque_exitoso = false; 
            break; 
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
    
    // If the 3-second precharge completes without faults, close the main contactor
    if (arranque_exitoso) {
        BMS_Set_MainContactor(true);
        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeño retardo mecánico
        BMS_Set_PrechargeRelay(false);
        ESP_LOGI(TAG, "Secuencia completada. Vehiculo energizado.");
    }

     // 3. NORMAL OPERATION
    while (arranque_exitoso) {
        // Monitor the emergency fault flag
        if (bms_emergency_fault == true) {
            ESP_LOGE(TAG, "¡Emergencia detectada! Saliendo del modo normal...");
            break; // Rompemos el bucle
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }

    // 4. EMERGENCY SHUTDOWN
    ESP_LOGE(TAG, "AISLANDO BATERIA. CORTANDO CONTACTORES...");
    BMS_Set_MainContactor(false);
    BMS_Set_PrechargeRelay(false);
    ESP_LOGI(TAG, "Relés abiertos físicamente. Tarea finalizada.");

    vTaskDelete(NULL); 
}