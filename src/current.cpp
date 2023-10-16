/**
 * @file current.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief BLDC current calculation operations file
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "current.h"

bool overCur = false;
QueueHandle_t currentQueue = xQueueCreate(1, sizeof(float));

void currentTask(void *pvParameters)
{
    int sampleCount = 0;
    float totalCurrent = 0; 
    while (1)
    {
        if (sampleCount < CURRENT_SAMPLE)
        {
            int amperHam = analogRead(CURRENT_PIN);
            totalCurrent += (0.0011 * (float)amperHam * (float)amperHam + 0.4262 * (float)amperHam + 18.603) / 1000.0;
            sampleCount++;
        }
        else
        {
            float amper = totalCurrent / (float)sampleCount;
            sampleCount = 0;
            totalCurrent = 0;
            xQueueOverwrite(currentQueue, &amper);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

float getCurrent()
{
    float retVal = -1;
    xQueuePeek(currentQueue, &retVal, portMAX_DELAY);
    return retVal;
}

bool checkOverCurrent()
{
    bool retVal = false;
    float amper = 0;
    xQueuePeek(currentQueue, &amper, portMAX_DELAY);
    if (overCur == true || amper > MAX_AMPER)
    {
        overCur = true;
        retVal = true;
    }
    return retVal;
}