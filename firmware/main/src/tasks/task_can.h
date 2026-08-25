/**
 * @file task_can.h
 * @brief Interfaz de comunicaciones CAN Bus.
 */

#ifndef TASK_CAN_H
#define TASK_CAN_H

#include "freertos/FreeRTOS.h"

void Task_BMS_CAN(void *pvParameters);

#endif // TASK_CAN_H