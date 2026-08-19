/**
 * @file bms_types.h
 * @brief Common data structures and enumerations for the BMS.
 */

#ifndef BMS_TYPES_H
#define BMS_TYPES_H

#include <stdint.h>

// Thermal state machine states (Stateflow)
typedef enum {
    THERMAL_IDLE,
    THERMAL_VENT_LOW,
    THERMAL_VENT_HIGH,
    THERMAL_EMERGENCY
} ThermalState_t;

// General BMS operating states
typedef enum {
    BMS_INIT,
    BMS_STANDBY,
    BMS_CHARGING,
    BMS_DISCHARGING,
    BMS_FAULT
} BMS_Status_t;

// Individual cell data structure
typedef struct {
    float voltage;
    float temperature;
    uint8_t id;
} CellData_t;

// Main telemetry structure (data transmitted over the CAN bus)
typedef struct {
    float soc;              // State of Charge (%)
    float sop;              // State of Power (%)
    float soh;              // State of Health (%)
    float current;          // Instantaneous current (A)
    float max_voltage;      // Maximum cell voltage (V)
    float min_voltage;      // Minimum cell voltage (V)
    float pack_temp;        // Average / hot-spot temperature (°C)
    ThermalState_t thermal_state;
    BMS_Status_t status;
} SystemTelemetry_t;

// Active balancing configuration (used for the Buck-Boost balancing topology)
typedef struct {
    bool active;            // Indicates whether balancing is active
    uint8_t source_cell;     // Source cell (highest voltage)
    uint8_t target_cell;    // Target cell (lowest voltage)
    float delta_v;          // Current voltage difference
} BalancingStatus_t;

// --- BATTERY PACK HARDWARE CONFIGURATION ---
#define NUM_SLAVES 3
#define CELLS_PER_SLAVE 10
#define TEMPS_PER_SLAVE 2
#define TOTAL_CELLS (NUM_SLAVES * CELLS_PER_SLAVE)
#define TOTAL_TEMPS (NUM_SLAVES * TEMPS_PER_SLAVE)

// --- RAW SENSOR DATA BUFFER ---
// The simulator (or the real AFE via SPI) writes the raw sensor readings here.
typedef struct {
    float cell_voltages[TOTAL_CELLS];  // Array of 30 cell voltages
    float temperatures[TOTAL_TEMPS];   // Array of 6 NTC temperature sensors
    float pack_voltage;                 // Total pack voltage (V)
    float pack_current;                 // Current measured by the shunt resistor (A)
    
    float slave_pcb_temperatures[NUM_SLAVES];
    bool is_simulating;                
} BMS_Raw_Data_t;

// Global variable declaration (accessible from any task including this header)
extern BMS_Raw_Data_t bms_raw_data;

// Global processed telemetry variable for the CAN bus
extern SystemTelemetry_t bms_telemetry;

// Global variable for active balancing status
extern BalancingStatus_t bms_balancing_status[NUM_SLAVES];

extern volatile bool bms_emergency_fault;

#endif // BMS_TYPES_H
