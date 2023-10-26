/**
 * @file rotation.h
 * @author Halit Cetin (halitcetin@live.com)
 * @brief MRotation calculation header file
 * @version 0.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ROTATION_H
#define ROTATION_H

#include <Arduino.h>
#include "definitions.h"

#define LEFT 0
#define RIGHT 1
#define STOP 2

/**
 * @brief Rotation interrupt
 * 
 */
extern void IRAM_ATTR rotationInterrupt();

/**
 * @brief Get the Rotation
 * 
 * @return int Rotation
 */
extern int getRotation();

#endif
