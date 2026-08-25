/**
 * @file main.c
 * @brief Firmware entry point.Initializes the hardware and starts the FreeRTOS scheduler.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "bms_hardware.h"
#include "bms_types.h"
#include "task_simulator.h"
#include "task_contactors.h"
#include "task_safety.h"
#include "task_termal.h"
#include "task_soc.h"
#include "task_balancing.h"
#include "task_can.h"


// Queue declarations (global so they can be accessed by all tasks)
QueueHandle_t xDataQueue;
volatile bool bms_emergency_fault = false;
SystemTelemetry_t bms_telemetry;
//Array físico de 3 balanceadores (uno para cada esclavo)
BalancingStatus_t bms_balancing_status[NUM_SLAVES];

void app_main(void) {
    // 1. Initialize the Hardware Abstraction Layer (HAL)
    BMS_Hardware_Init();

    // 2. Create the queues (buffer for 10 telemetry samples)
    xDataQueue = xQueueCreate(10, sizeof(SystemTelemetry_t));

    if (xDataQueue == NULL) {
         // Critical error handling if there is not enough memory to create the queue
        return;
    }

    // 3. Create and start the FreeRTOS tasks

    // Safety watchdog task (highest priority: 15)
    xTaskCreate(Task_SafetyMonitor, "SafetyTask", 2048, NULL, 15, NULL);

    // Contactor management task (high priority: 10)
    // Since it is safety-critical for the racing motorcycle, it is assigned a high priority
    xTaskCreate(Task_ContactorManager, "ContactorMgr", 2048, NULL, 10, NULL);

    // CAN Bus communications task (High priority: 9)
    xTaskCreate(Task_BMS_CAN, "CANTask", 3072, NULL, 9, NULL);

    // SoC estimation task (Medium-high priority: 7)
    // IMPORTANT: 4096 bytes allocated to prevent stack overflow with floating-point math
    xTaskCreate(Task_BMS_StateEstimation, "SoCTask", 4096, NULL, 7, NULL);

    // Thermal management task (medium priority: 6)
    xTaskCreate(Task_ThermalManager, "ThermalTask", 2048, NULL, 6, NULL);

    // Data simulator task (medium priority: 5)
    // A 4096-byte stack is allocated because it uses logging functions (ESP_LOGI) and rand()
    xTaskCreate(Task_BMS_Simulator, "SimulatorTask", 4096, NULL, 5, NULL);

    // NUEVO: Active cell balancing task (Low priority: 4)
    // Runs every 1 second, so it doesn't need to block critical systems
    xTaskCreate(Task_BMS_Balancing, "BalancingTask", 3072, NULL, 4, NULL);

    

    // From this point onward, the FreeRTOS scheduler takes control.
    // app_main finishes its sequential execution, and FreeRTOS manages the tasks indefinitely.
}