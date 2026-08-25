/**
 * @file task_termal.h
 * @brief Thermal management and forced cooling control for the battery pack.
 */

#ifndef TASK_THERMAL_H
#define TASK_THERMAL_H

/** * @brief Task responsible for monitoring battery pack temperatures and
  * controlling the cooling fan PWM using a linear control law with hysteresis.
  */
void Task_ThermalManager(void *pvParameters);

#endif // TASK_THERMAL_H