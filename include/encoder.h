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

#define ENCODER_NO_ERROR 0
#define ENCODER_STEP_READING_ERROR 1
#define ENCODER_STEP_MISSING_ERROR 2

/**
 * @brief Get the encoder step
 * 
 * @return int Encoder step
 */
extern int getEncoderStep();

/**
 * @brief Get the distance as a count of encoder steps
 * 
 * @return int Motor distance
 */
extern int getDistance();

/**
 * @brief Reset the distance that motor has gone
 * 
 */
extern void resetDistance();

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