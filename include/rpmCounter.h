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
#include "encoder.h"

#define MIN_RPM 0
#define MAX_RPM 50
#define MIN_CAPTURE_DUTY 0
#define MAX_CAPTURE_DUTY 100
#define RPM_SAMPLE_COUNT 100
#define RPM_CAPTURE_RESET_COUNT 500

/**
 * @brief RPM decrease/increase request interrupt
 * 
 */
extern void IRAM_ATTR captureRpmRequest();

/**
 * @brief Get the requested RPM
 * 
 * @return int Requested RPM
 */
extern int getRpmRequest();

#endif
