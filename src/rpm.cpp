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

static SimpleKalmanFilter rpmKalmanFilter(1, 1, 0.01);
static QueueHandle_t rpmQueue = xQueueCreate(1, sizeof(float));
static unsigned long lastStepTime = 0;
static unsigned long currentMillis = 0;
static unsigned long stepTimeDifference = 0;

void calculateRpmTask(void *pvParameters)
{
    while (1)
    {
        currentMillis = millis();
        stepTimeDifference = currentMillis - lastStepTime;
        int distance = getDistance();
        resetDistance();
        float rpm = abs((float)distance / (float)(stepTimeDifference)*MINUTE_IN_MS / STEP_TO_REVOLUTION);
        float estimatedRpm = rpmKalmanFilter.updateEstimate(rpm);
        xQueueOverwrite(rpmQueue, &estimatedRpm);
        lastStepTime = currentMillis;
        Serial.println(String(rpm) + " " + String(estimatedRpm));
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

float getRpm()
{
    float retVal = -1;
    xQueuePeek(rpmQueue, &retVal, portMAX_DELAY);
    return retVal;
}