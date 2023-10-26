/**
 * @file rotation.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief MRotation calculation file
 * @version 0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "rotation.h"

static portMUX_TYPE rotationCounterMux = portMUX_INITIALIZER_UNLOCKED;
static volatile int rotation = STOP;

void IRAM_ATTR rotationInterrupt()
{
    portENTER_CRITICAL_ISR(&rotationCounterMux);

    int closeState = digitalRead(CLOSE_PIN);
    int stopState = digitalRead(STOP_PIN);

    if (closeState == HIGH && stopState == LOW)
    {
        rotation = LEFT;
    }
    else if (closeState == LOW && stopState == HIGH)
    {
        rotation = RIGHT;
    }
    else if (closeState == HIGH && stopState == HIGH)
    {
        rotation = STOP;
    }

    portEXIT_CRITICAL_ISR(&rotationCounterMux);
}

int getRotation()
{
    int retVal;
    portENTER_CRITICAL(&rotationCounterMux);
    retVal = rotation;
    portEXIT_CRITICAL(&rotationCounterMux);
    return retVal;
}
