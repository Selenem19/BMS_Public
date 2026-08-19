#include "bms_hardware.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define PIN_PRECHARGE    16  // Blue LED (precharge contactor)
#define PIN_MAIN         4   // Yellow LED (main contactor)
#define PIN_FAN_PWM      2   // Green LED (integrated PWM fan indicator)
#define PIN_THERMAL_LOCK 17  // Red LED (thermal lock indicator)

void BMS_Hardware_Init(void) {
    // --- 1. DIGITAL GPIO CONFIGURATION ---
    gpio_set_level(PIN_PRECHARGE, 0);
    gpio_set_level(PIN_MAIN, 0);
    gpio_set_level(PIN_THERMAL_LOCK, 0); 

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_PRECHARGE) | (1ULL << PIN_MAIN) | (1ULL << PIN_THERMAL_LOCK), 
        .mode = GPIO_MODE_OUTPUT,              
        .pull_up_en = GPIO_PULLUP_DISABLE,     
        .pull_down_en = GPIO_PULLDOWN_DISABLE, 
        .intr_type = GPIO_INTR_DISABLE         
    };
    gpio_config(&io_conf);

     // --- 2. FAN PWM CONFIGURATION (LEDC) ---
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT, 
        .freq_hz          = 25000,             
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,               
        .gpio_num   = PIN_FAN_PWM,     
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
}

void BMS_Set_PrechargeRelay(bool state) { gpio_set_level(PIN_PRECHARGE, state); }
void BMS_Set_MainContactor(bool state) { gpio_set_level(PIN_MAIN, state); }
void BMS_Set_ThermalLockLED(bool state) { gpio_set_level(PIN_THERMAL_LOCK, state); }

void BMS_Set_FanPWM(float duty_cycle) {
    uint32_t duty = (uint32_t)((duty_cycle / 100.0f) * 1023.0f);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}