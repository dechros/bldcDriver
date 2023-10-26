/**
 * @file rpm.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief RPM request calculation header file
 * @version 0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef RPMCOUNTER_H
#define RPMCOUNTER_H

#include <Arduino.h>
#include "definitions.h"

#define MIN_RPM 0
#define MAX_RPM 50
#define DEBOUNCE_DELAY_MICROS 100000

/**
 * @brief RPM increase request interrupt
 * 
 */
extern void IRAM_ATTR increaseRpm();

/**
 * @brief RPM decrease request interrupt
 * 
 */
extern void IRAM_ATTR decreaseRpm();

/**
 * @brief Get the requested RPM
 * 
 * @return int Requested RPM
 */
extern int getRpmRequest();

#endif
