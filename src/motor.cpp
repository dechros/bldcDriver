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

static QueueHandle_t rotationQueue = xQueueCreate(1, sizeof(double));
static QueueHandle_t rpmQueue = xQueueCreate(1, sizeof(int));

/**
 * @brief The function that drives motor according to encoder step
 *
 */
static void driveMotor(int pinRotation, int pinDuty);

void motorTask(void *pvParameters)
{
    float duty = 0;
    int currentRotation = STOP;
    encoderInterrupt();
    while (1)
    {
        int targetRpm = 0;
        int requestedRotation = 0;
        float rpm = getRpm();
        xQueuePeek(rotationQueue, &requestedRotation, portMAX_DELAY);
        xQueuePeek(rpmQueue, &targetRpm, portMAX_DELAY);
        if (currentRotation != requestedRotation)
        {
            duty -= DUTY_RAMP_VAL;
            if (duty <= MIN_DUTY)
            {
                currentRotation = requestedRotation;
            }
        }
        else
        {
            if (rpm < targetRpm)
            {
                duty += DUTY_RAMP_VAL;
            }
            else if (rpm >= targetRpm)
            {
                duty -= DUTY_RAMP_VAL;
            }
        }
        float current = getCurrent();
        if (current >= DANGER_AMPER)
        {
            duty -= DUTY_RAMP_VAL * 5;
        }
        if (checkCurrentError() == true)
        {
            duty = MIN_DUTY;
        }
        if (duty < MIN_DUTY)
        {
            duty = MIN_DUTY;
        }
        else if (duty > MAX_DUTY)
        {
            duty = MAX_DUTY;
        }
        serialWrite("Tar Rot : " + String(requestedRotation) + " | Cur Rot : " + String(currentRotation) + " | Tar RPM : " + String(targetRpm) + " | Cur RPM : " + String(rpm) + " | Duty : " + String(duty) + " | Amper : " + String(current));
        driveMotor(currentRotation, (int)duty);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void driveMotor(int pinRotation, int pinDuty)
{
    int highPwmDuty = 0;
    int lowPwmDuty = 0;

    if (pinRotation == LEFT)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES + pinDuty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES - pinDuty;
    }
    else if (pinRotation == RIGHT)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES - pinDuty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES + pinDuty;
    }
    else
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES;
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
