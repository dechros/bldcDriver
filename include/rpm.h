/**
 * @file rpm.h
 * @author Halit Cetin (halitcetin@live.com)
 * @brief BLDC RPM calculation operations header file
 * @version 0.1
 * @date 2023-10-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <Arduino.h>
#include <SimpleKalmanFilter.h>
#include "globals.h"
#include "encoder.h"

#ifndef RPM_H
#define RPM_H

#define STEP_TO_REVOLUTION 476
#define MINUTE_IN_MS 60000.0f

/**
 * @brief Get RPM value of the motor
 * 
 * @return float RPM value of the motor
 */
extern float getRpm();

/**
 * @brief The task that calculates motor RPM
 * 
 * @param pvParameters Parameter to check if the task is created correctly
 */
extern void calculateRpmTask(void *pvParameters);

#endif