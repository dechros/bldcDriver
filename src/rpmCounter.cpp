/**
 * @file rpm.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief RPM request calculation file
 * @version 0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "rpmCounter.h"

static portMUX_TYPE rpmCounterMux = portMUX_INITIALIZER_UNLOCKED;

static volatile int rpm = 0;
static unsigned long lastUpdateTime = 0;

void IRAM_ATTR increaseRpm()
{
    portENTER_CRITICAL_ISR(&rpmCounterMux);
    unsigned long currentTime = micros();
    if (currentTime - lastUpdateTime >= DEBOUNCE_DELAY_MICROS)
    {
        rpm++;
        if (rpm > MAX_RPM)
        {
            rpm = MAX_RPM;
        }
        lastUpdateTime = currentTime;
    }
    portEXIT_CRITICAL_ISR(&rpmCounterMux);
}

void IRAM_ATTR decreaseRpm()
{
    portENTER_CRITICAL_ISR(&rpmCounterMux);
    unsigned long currentTime = micros();
    if (currentTime - lastUpdateTime >= DEBOUNCE_DELAY_MICROS)
    {
        rpm--;
        if (rpm < MIN_RPM)
        {
            rpm = MIN_RPM;
        }
        lastUpdateTime = currentTime;
    }
    portEXIT_CRITICAL_ISR(&rpmCounterMux);
}

int getRpmRequest()
{
    int retVal;
    portENTER_CRITICAL(&rpmCounterMux);
    retVal = rpm;
    portEXIT_CRITICAL(&rpmCounterMux);
    return retVal;
}