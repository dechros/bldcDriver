/**
 * @file motor.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief Motor driving operations header file
 * @version 0.1
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <PID_v1.h>
#include "encoder.h"
#include "current.h"
#include "serial.h"

#define MAX_RPM 40
#define MIN_DUTY 0
#define MAX_DUTY 500
#define RAMP_TIME 3
#define KP 2.0
#define KI 9.0
#define KD 0.002

/**
 * @brief Motor driving task
 * 
 * @param pvParameters 
 */
extern void motorTask(void *pvParameters);

/**
 * @brief Set the motor rotation
 * 
 * @param pinRotation rotation of the motor
 */
extern void setRotation(int pinRotation);

/**
 * @brief Set the motor RPM
 * 
 * @param pinDuty RPM of the motor
 */
extern void setRpm(int pinRpm);

#endif