/**
 * @file rpm.c
 * @author Halit Cetin (halitcetin@live.com)
 * @brief BLDC RPM calculation operations file
 * @version 0.1
 * @date 2023-10-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "rpm.h"

QueueHandle_t rpmQueue = xQueueCreate(1, sizeof(float));
unsigned long lastStepTime = 0;
unsigned long currentMillis = 0;
unsigned long stepTimeDifference = 0;

void calculateRpmTask(void *pvParameters)
{
    int sampleCount = 0;
    float totalRpm = 0;
    while (1)
    {
        if (sampleCount < RPM_SAMPLE)
        {
            currentMillis = millis();
            stepTimeDifference = currentMillis - lastStepTime;
            int distance = getDistance();
            resetDistance();
            totalRpm += abs((float)distance / (float)(stepTimeDifference)*MINUTE_IN_MS / STEP_TO_REVOLUTION);
            lastStepTime = currentMillis;
            sampleCount++;
        }
        else
        {
            float rpm = totalRpm / (float)sampleCount;
            sampleCount = 0;
            totalRpm = 0;
            xQueueOverwrite(rpmQueue, &rpm);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

float getRpm()
{
    float retVal = -1;
    xQueuePeek(rpmQueue, &retVal, portMAX_DELAY);
    return retVal;
}