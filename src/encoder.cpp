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
static volatile int distance = -1;
static volatile int encoderErrorState = ENCODER_NO_ERROR;

/**
 * @brief Returns the indes of encoder step as they are sequential
 * 
 * @return int Encoder step index in the sequence
 */
static int findEncoderStepIndex();

int getEncoderStep()
{
    int retVal = -1;
    portENTER_CRITICAL(&encoderInterruptMux);
    retVal = encoderStep;
    portEXIT_CRITICAL(&encoderInterruptMux);
    return retVal;
}

int getDistance()
{
    int retVal = -1;
    portENTER_CRITICAL(&encoderInterruptMux);
    retVal = distance;
    portEXIT_CRITICAL(&encoderInterruptMux);
    return retVal;
}

void resetDistance()
{
    portENTER_CRITICAL(&encoderInterruptMux);
    distance = 0;
    portEXIT_CRITICAL(&encoderInterruptMux);
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

    if (encoderStep == 0 || encoderStep == 111)
    {
        encoderErrorState = ENCODER_STEP_READING_ERROR;
    }
    else
    {
        int newIndex = findEncoderStepIndex();
        static int oldIndex = newIndex;

        int indexDiff = newIndex - oldIndex;

        if (indexDiff == 1 || indexDiff == -5)
        {
            distance++;
        }
        else if (indexDiff == -1 || indexDiff == 5)
        {
            distance--;
        }
        else if (indexDiff != 0)
        {
            encoderErrorState = ENCODER_STEP_MISSING_ERROR;
        }

        oldIndex = newIndex;
    }

    portEXIT_CRITICAL(&encoderInterruptMux);
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