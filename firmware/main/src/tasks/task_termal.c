#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bms_types.h"
#include "bms_hardware.h"
#include "esp_log.h"

static bool cooling_hysteresis = false;
static bool thermal_latch = false; 

void Task_ThermalManager(void *pvParameters) {
    while(1) {
        float t_max = 0.0f;
        for(int i=0; i<TOTAL_TEMPS; i++) {
            if(bms_raw_data.temperatures[i] > t_max) t_max = bms_raw_data.temperatures[i];
        }

        // 1. CRITICAL THERMAL CONDITION (>= 60 °C)
        if (t_max >= 60.0f) { 
            thermal_latch = true;        
            bms_emergency_fault = true;  
            BMS_Set_FanPWM(100.0f);      
            BMS_Set_ThermalLockLED(true); // Turn on the thermal fault indicator
        } 
        // 2. LATCHED STATE (Cooling down after an emergency)
        else if (thermal_latch == true) {
            BMS_Set_FanPWM(100.0f); 
            
            // Clear the thermal latch once the safe reset temperature is reached
            if (t_max <= 50.0f) {
                thermal_latch = false; 
                BMS_Set_ThermalLockLED(false); // Turn off the indicator (system can be restarted)
            }
        }
        // 3. POWER LIMITING REGION (50 °C to 60 °C)
        else if (t_max >= 50.0f) { 
            BMS_Set_FanPWM(100.0f);
        }
        // 4. PROPORTIONAL COOLING (35 °C to 50 °C)
        else if (t_max >= 35.0f || (cooling_hysteresis && t_max > 32.0f)) { 
            float duty = 30.0f + 4.67f * (t_max - 35.0f);

            if (duty > 100.0f) {
                duty = 100.0f;
            }
            
            BMS_Set_FanPWM(duty);
            cooling_hysteresis = true;
        }
        
        // 5. NORMAL OPERATION (< 35 °C)
        else { 
            BMS_Set_FanPWM(0.0f);
            cooling_hysteresis = false;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
}