/**
 * @file bms_hardware.h
 * @brief Hardware Abstraction Layer (HAL) interface for the BMS.
 */

#ifndef BMS_HARDWARE_H
#define BMS_HARDWARE_H

#include <stdbool.h>
#include <stdint.h>
#include "bms_types.h"

// --- Initialization ---
void BMS_Hardware_Init(void);

// --- Contactor Control ---
void BMS_Set_PrechargeRelay(bool state);
void BMS_Set_MainContactor(bool state);

// --- Actuator Control (Cooling and Balancing) ---
void BMS_Set_FanPWM(float duty_cycle); // PWM duty cycle (0.0 to 1.0)
void BMS_Set_BalancingSwitch(uint8_t cell_index, bool state);

// Dashboard thermal lock indicator
void BMS_Set_ThermalLockLED(bool state);

// --- Sensor Data Acquisition ---
// Returns the voltage of the specified cell (ADC)
float BMS_Read_CellVoltage(uint8_t cell_index);

// Returns the temperature measured by the specified NTC sensor
float BMS_Read_Temperature(uint8_t sensor_index);

// Returns the bus current measured by the Hall-effect sensor
float BMS_Read_Current(void);

#endif // BMS_HARDWARE_H

