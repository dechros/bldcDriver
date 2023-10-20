/**
 * @file serial.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief Serial operations file
 * @version 0.1
 * @date 2023-10-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "serial.h"

static QueueHandle_t inputQueue = xQueueCreate(10, MAX_INPUT_LENGTH);
static QueueHandle_t outputQueue = xQueueCreate(10, MAX_INPUT_LENGTH);

void serialTask(void *param)
{
    char inputBuffer[MAX_INPUT_LENGTH];
    char outputBuffer[MAX_INPUT_LENGTH];
    while (true)
    {
        if (Serial.available())
        {
            int bytesRead = Serial.readBytesUntil('\n', inputBuffer, MAX_INPUT_LENGTH);
            if (bytesRead > 0)
            {
                inputBuffer[bytesRead] = '\0';
                xQueueSend(inputQueue, inputBuffer, portMAX_DELAY);
            }
        }
        if (xQueueReceive(outputQueue, outputBuffer, 0))
        {
            Serial.println(outputBuffer);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

String serialRead()
{
    char inputBuffer[MAX_INPUT_LENGTH];
    if (xQueueReceive(inputQueue, inputBuffer, 0))
    {
        return String(inputBuffer);
    }
    return "";
}

void serialWrite(const char *message)
{
    xQueueSend(outputQueue, message, portMAX_DELAY);
}

void serialWrite(String message)
{
    xQueueSend(outputQueue, message.c_str(), portMAX_DELAY);
}
