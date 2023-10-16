/**
 * @file current.h
 * @author Halit Cetin (halitcetin@live.com)
 * @brief BLDC current calculation operations header file
 * @version 0.1
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "globals.h"
#include "definitions.h"

#define CURRENT_SAMPLE 250
#define MAX_AMPER 3

/**
 * @brief Current reading task for the motor
 * 
 * @param pvParameters Parameter to check if the task is created correctly
 */
extern void currentTask(void *pvParameters);

/**
 * @brief Get current as Amper
 * 
 * @return float Current value as Amper
 */
extern float getCurrent();

/**
 * @brief Checks if the current is over the limit
 * 
 * @return true Overcurrent is present
 * @return false Overcurrent is not present
 */
extern bool checkOverCurrent();

#endif