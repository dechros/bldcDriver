/**
 * @file motor.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief Motor driving operations file
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "motor.h"

static QueueHandle_t rotationQueue = xQueueCreate(1, sizeof(int));
static QueueHandle_t rpmQueue = xQueueCreate(1, sizeof(int));

/**
 * @brief The function that drives motor according to encoder step.
 *
 */
static void driveMotor(int motorRotation, int duty);

void motorTask(void *pvParameters)
{
    int duty = 0;
    encoderInterrupt();
    while (1)
    {
        float currentRpm = getRpm();

        int requestedRpm = 0;
        int requestedRotation = 0;

        xQueuePeek(rotationQueue, &requestedRotation, portMAX_DELAY);
        xQueuePeek(rpmQueue, &requestedRpm, portMAX_DELAY);

        if (currentRpm < requestedRpm)
        {
            duty++;
        }
        else if (currentRpm > requestedRpm)
        {
            duty--;
        }

        if (duty > MAX_DUTY)
        {
            duty = MAX_DUTY;
        }
        else if (duty < MIN_DUTY)
        {
            duty = MIN_DUTY;
        }
        /*
        if (checkOverCurrent() == true)
        {
            duty = MIN_DUTY;
        }
        */
        Serial.println(requestedRpm);
        driveMotor(requestedRotation, duty);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void driveMotor(int motorRotation, int duty)
{
    int highPwmDuty = 0;
    int lowPwmDuty = 0;

    if (motorRotation == 0)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES + duty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES - duty;
    }
    if (motorRotation == 1)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES - duty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES + duty;
    }

    int encoderStep = getEncoderStep();

    switch (encoderStep)
    {
    case 101:
        ledcWrite(PWM_CHANNEL_1, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, highPwmDuty);
        break;
    case 100:
        ledcWrite(PWM_CHANNEL_1, highPwmDuty);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, MOTOR_DRIVE_DUTY_RES);
        break;
    case 110:
        ledcWrite(PWM_CHANNEL_1, highPwmDuty);
        ledcWrite(PWM_CHANNEL_2, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    case 10:
        ledcWrite(PWM_CHANNEL_1, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_2, highPwmDuty);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    case 11:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, highPwmDuty);
        ledcWrite(PWM_CHANNEL_3, MOTOR_DRIVE_DUTY_RES);
        break;
    case 1:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_3, highPwmDuty);
        break;
    default:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    }
}

void setRotation(int pinRotation)
{
    xQueueOverwrite(rotationQueue, &pinRotation);
}

void setRpm(int pinRpm)
{
    xQueueOverwrite(rpmQueue, &pinRpm);
}