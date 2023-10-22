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

#define AMPER_NO_ERROR 0
#define AMPER_OVER_CURRENT 1
#define CURRENT_SAMPLE 25
#define MAX_AMPER 6

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
extern bool checkCurrentError();

#endif