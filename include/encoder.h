/**
 * @file encoder.h
 * @author Halit Cetin (halitcetin@live.com)
 * @brief Encoder operations header file
 * @version 0.1
 * @date 2023-10-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <esp_attr.h>
#include "definitions.h"
#include "rpmCounter.h"

#define ENCODER_NO_ERROR 0
#define ENCODER_STEP_READING_ERROR 1
#define ENCODER_STEP_MISSING_ERROR 2
#define STEP_TO_REVOLUTION 476
#define RPM_RESET_COUNT 100

/**
 * @brief Get the RPM from interrupt
 * 
 * @return float RPM of two different steps 
 */
extern float getRpm();

/**
 * @brief Get the encoder step
 * 
 * @return int Encoder step
 */
extern int getEncoderStep();

/**
 * @brief Get the encoder error state
 * 
 * @return int Encoder error states
 */
extern int getEncoderErrorState();

/**
 * @brief Encoder A B C pin interrupt to get the step
 * 
 */
extern void IRAM_ATTR encoderInterrupt();

#endif