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
static volatile bool lastDirState = LOW;
static volatile unsigned long highStartTime = 0;
static volatile unsigned long lowStartTime = 0;
static volatile unsigned long highTime = 0;
static volatile unsigned long lowTime = 0;
static volatile int rpmTotal = 0;
static volatile int rpmCounter = 0;

void IRAM_ATTR captureRpmRequest()
{
    portENTER_CRITICAL_ISR(&rpmCounterMux);
    bool currentDirState = digitalRead(OPEN_PIN);
    if (currentDirState != lastDirState)
    {
        if (currentDirState == HIGH)
        {
            highStartTime = micros();
            if (lowStartTime != 0)
            {
                lowTime += highStartTime - lowStartTime;
            }
        }
        else
        {
            lowStartTime = micros();
            if (highStartTime != 0)
            {
                highTime += lowStartTime - highStartTime;
            }
        }
        if (lowTime > 0 && highTime > 0)
        {
            int duty = (highTime * 100) / (lowTime + highTime);
            int localRpm = map(duty, MIN_CAPTURE_DUTY, MAX_CAPTURE_DUTY, MIN_RPM, MAX_RPM);
            rpmTotal += localRpm;
            rpmCounter++;
            highStartTime = 0;
            lowStartTime = 0;
            highTime = 0;
            lowTime = 0;
        }
        lastDirState = currentDirState;
    }
    portEXIT_CRITICAL_ISR(&rpmCounterMux);
}

int getRpmRequest()
{
    int retVal = -1;
    static float oldRpm = 0;
    float rpmTotalLocal = 0;
    float rpmCounterLocal = 0;
    portENTER_CRITICAL(&rpmCounterMux);
    if (rpmCounter >= RPM_SAMPLE_COUNT)
    {
        rpmTotalLocal = rpmTotal;
        rpmCounterLocal = rpmCounter;
        rpmCounter = 0;
        rpmTotal = 0;
    }
    portEXIT_CRITICAL(&rpmCounterMux);
    if (rpmTotalLocal > 0)
    {
        float rpm = rpmTotalLocal / rpmCounterLocal;
        oldRpm = rpm;
        retVal = (int)rpm;
    }
    else
    {
        retVal = (int)oldRpm;
    }
    return retVal;
}
