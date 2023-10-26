/**
 * @file encoder.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief Encoder operations file
 * @version 0.1
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "encoder.h"

static portMUX_TYPE encoderInterruptMux = portMUX_INITIALIZER_UNLOCKED;

static volatile int encoderStep = -1;
static volatile int distance = 0;
static volatile int encoderErrorState = ENCODER_NO_ERROR;
static volatile unsigned long lastStepTime = 0;
static volatile float rpm = 0;

/**
 * @brief Returns the indes of encoder step as they are sequential
 *
 * @return int Encoder step index in the sequence
 */
static int findEncoderStepIndex();

/**
 * @brief Checks if the RPM value is too high
 *
 * @param pinRpm Current RPM value
 * @return float Corrected RPM value
 */
static float checkRpmAbsurdity(float pinRpm);

/**
 * @brief Checks and resets if RPM values are sequential
 *
 * @param float Current RPM
 * @return float Corrected RPM value
 */
static float checkRpmResetTime(float pinRpm);

float getRpm()
{
    float retVal = -1;
    portENTER_CRITICAL(&encoderInterruptMux);
    rpm = checkRpmAbsurdity(rpm);
    rpm = checkRpmResetTime(rpm);
    retVal = rpm;
    portEXIT_CRITICAL(&encoderInterruptMux);
    return retVal;
}

int getEncoderStep()
{
    int retVal = -1;
    portENTER_CRITICAL(&encoderInterruptMux);
    retVal = encoderStep;
    portEXIT_CRITICAL(&encoderInterruptMux);
    return retVal;
}

int getEncoderErrorState()
{
    int retVal = -1;
    portENTER_CRITICAL(&encoderInterruptMux);
    retVal = encoderErrorState;
    portEXIT_CRITICAL(&encoderInterruptMux);
    return retVal;
}

void IRAM_ATTR encoderInterrupt()
{
    portENTER_CRITICAL(&encoderInterruptMux);
    int encoderaState = digitalRead(ENCODER_A_PIN);
    int encoderbState = digitalRead(ENCODER_B_PIN);
    int encodercState = digitalRead(ENCODER_C_PIN);
    encoderStep = encoderaState * 100 + encoderbState * 10 + encodercState;
    unsigned long stepTimeDifference = 0;
    if (encoderStep == 0 || encoderStep == 111)
    {
        encoderErrorState = ENCODER_STEP_READING_ERROR;
    }
    else
    {
        int newIndex = findEncoderStepIndex();
        static int oldIndex = newIndex;

        int indexDiff = newIndex - oldIndex;

        if (indexDiff == 1 || indexDiff == -5 || indexDiff == -1 || indexDiff == 5)
        {
            unsigned long currentTime = micros();
            stepTimeDifference = currentTime - lastStepTime;
            lastStepTime = currentTime;
            rpm = (1.0 / stepTimeDifference) * 60000000.0 / STEP_TO_REVOLUTION;
        }
        else if (indexDiff != 0)
        {
            encoderErrorState = ENCODER_STEP_MISSING_ERROR;
        }
        oldIndex = newIndex;
    }
    portEXIT_CRITICAL(&encoderInterruptMux);
}

static float checkRpmResetTime(float pinRpm)
{
    static int rpmCount = 0;
    static float oldRpm = 0;
    if (oldRpm == pinRpm)
    {
        rpmCount++;
    }
    else
    {
        rpmCount = 0;
    }
    if (rpmCount > RPM_RESET_COUNT)
    {
        rpm = 0;
        pinRpm = 0;
        rpmCount = 0;
    }
    oldRpm = pinRpm;
    return pinRpm;
}

static float checkRpmAbsurdity(float pinRpm)
{
    static float oldRpm = pinRpm;
    if (pinRpm > MAX_RPM + 10)
    {
        pinRpm = oldRpm;
    }
    oldRpm = pinRpm;
    return pinRpm;
}

static int findEncoderStepIndex()
{
    switch (encoderStep)
    {
    case 10:
        return 0;
    case 11:
        return 1;
    case 1:
        return 2;
    case 101:
        return 3;
    case 100:
        return 4;
    case 110:
        return 5;
    default:
        return -1;
    }
}