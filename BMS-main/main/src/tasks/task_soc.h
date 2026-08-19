/**
 * @file task_soc.h
 * @brief State of Charge (SoC) estimation using Coulomb Counting.
 */

#ifndef TASK_SOC_H
#define TASK_SOC_H

/**
 * @brief Task responsible for integrating the current over time
 * to accurately estimate the remaining battery charge.
 */
void Task_BMS_StateEstimation(void *pvParameters);

#endif // TASK_SOC_H