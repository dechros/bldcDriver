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
static QueueHandle_t rpmQueue = xQueueCreate(1, sizeof(double));

static double currentRpm = 0;
static double oldRpm = 0;
static int rpmCount = 0;

static double kp = 2.0;
static double ki = 8.0;
static double kd = 0.001;

/**
 * @brief The function that drives motor according to encoder step
 *
 */
static void driveMotor(int motorRotation, int duty);

/**
 * @brief Checks and resets if RPM values are sequential
 *
 */
static void checkRpmResetTime();

void motorTask(void *pvParameters)
{
    double duty = 0;
    double requestedRpm = 0;
    int requestedRotation = 0;
    PID myPID(&currentRpm, &duty, &requestedRpm, kp, ki, kd, DIRECT);
    myPID.SetMode(AUTOMATIC)
    myPID.SetOutputLimits(MIN_DUTY, MAX_DUTY);
    encoderInterrupt();
    while (1)
    {
        currentRpm = (double)getRpm();
        checkRpmResetTime();
        xQueuePeek(rotationQueue, &requestedRotation, portMAX_DELAY);
        xQueuePeek(rpmQueue, &requestedRpm, portMAX_DELAY);
        String outputString = String(requestedRpm, 2) + " " + String(currentRpm, 2);
        serialWrite(outputString.c_str());
        myPID.Compute();
        /*
        if (checkOverCurrent() == true)
        {
            duty = MIN_DUTY;
        }
        */
        driveMotor(requestedRotation, (int)duty);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void checkRpmResetTime()
{
    if (oldRpm == currentRpm)
    {
        rpmCount++;
    }
    if (rpmCount > 250)
    {
        resetRpm();
        currentRpm = 0;
        rpmCount = 0;
    }
    oldRpm = currentRpm;
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

void setRpm(double pinRpm)
{
    xQueueOverwrite(rpmQueue, &pinRpm);
}