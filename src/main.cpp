/**
 * @file main.cpp
 * @author Halit Cetin (halitcetin@live.com)
 * @brief BLDC driver main program file
 * @version 0.1
 * @date 2023-10-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "definitions.h"
#include "globals.h"
#include "motor.h"
#include "serial.h"
#include "rpmCounter.h"
#include "rotation.h"

#include <soc/rtc.h>
#include <rom/rtc.h>
#include <driver/mcpwm.h>
#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <esp_intr_alloc.h>
#include <driver/periph_ctrl.h>
#include <SimpleKalmanFilter.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

portMUX_TYPE pwmMutex = portMUX_INITIALIZER_UNLOCKED;
QueueHandle_t dutyQueue = xQueueCreate(1, sizeof(int));
volatile bool lastDirState = LOW;
volatile unsigned long highStartTime = 0;
volatile unsigned long lowStartTime = 0;
volatile unsigned long highTime = 0;
volatile unsigned long lowTime = 0;

void IRAM_ATTR pwmInterrupt()
{
    portENTER_CRITICAL_ISR(&pwmMutex);
    bool currentDirState = digitalRead(DIR_PIN);
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
            BaseType_t higherPriorityTaskWoken = pdFALSE;
            xQueueOverwriteFromISR(dutyQueue, &duty, &higherPriorityTaskWoken);
            highStartTime = 0;
            lowStartTime = 0;
            highTime = 0;
            lowTime = 0;
        }
        lastDirState = currentDirState;
    }
    portEXIT_CRITICAL_ISR(&pwmMutex);
}

void loop()
{
    int localDuty = 0;
    if (xQueuePeek(dutyQueue, &localDuty, pdMS_TO_TICKS(1)) == pdTRUE)
    {
        Serial.println(localDuty);
    }
    vTaskDelay(pdMS_TO_TICKS(1));
}

void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT);
    pinMode(ENCODER_B_PIN, INPUT);
    pinMode(ENCODER_C_PIN, INPUT);
    pinMode(FAULT_PIN, INPUT);
    pinMode(CLOSE_PIN, INPUT);
    pinMode(FTS_PIN, INPUT);
    pinMode(STOP_PIN, INPUT);
    pinMode(OPEN_PIN, INPUT);
    pinMode(DIR_PIN, INPUT);

    pinMode(H1_PIN, OUTPUT);
    pinMode(H2_PIN, OUTPUT);
    pinMode(H3_PIN, OUTPUT);
    pinMode(L1_PIN, OUTPUT);
    pinMode(L2_PIN, OUTPUT);
    pinMode(L3_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(TORK_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_C_PIN), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(FTS_PIN), decreaseRpm, RISING);
    attachInterrupt(digitalPinToInterrupt(OPEN_PIN), increaseRpm, RISING);
    attachInterrupt(digitalPinToInterrupt(CLOSE_PIN), rotationInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(STOP_PIN), rotationInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(DIR_PIN), pwmInterrupt, CHANGE);

    ledcSetup(PWM_CHANNEL_1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(PWM_CHANNEL_2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(PWM_CHANNEL_3, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(PWM_CHANNEL_4, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

    ledcAttachPin(H1_PIN, PWM_CHANNEL_1);
    ledcAttachPin(L1_PIN, PWM_CHANNEL_1);
    ledcAttachPin(H2_PIN, PWM_CHANNEL_2);
    ledcAttachPin(L2_PIN, PWM_CHANNEL_2);
    ledcAttachPin(H3_PIN, PWM_CHANNEL_3);
    ledcAttachPin(L3_PIN, PWM_CHANNEL_3);
    ledcAttachPin(TORK_PIN, PWM_CHANNEL_4);

    GPIO.func_out_sel_cfg[L1_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L2_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L3_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[H1_PIN].inv_sel = 0;
    GPIO.func_out_sel_cfg[H2_PIN].inv_sel = 0;
    GPIO.func_out_sel_cfg[H3_PIN].inv_sel = 0;

    xTaskCreatePinnedToCore(currentTask, "currentTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(motorTask, "motorTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(serialTask, "serialTask", 2048, NULL, 1, NULL, 0);

    digitalWrite(STATUS_LED_PIN, HIGH);
    ledcWrite(PWM_CHANNEL_4, 3900);
    serialWrite("Setup Completed.");
}

void mainCode()
{
    int rpmRequest = getRpmRequest();
    int rotationInterrupt = getRotation();
    setRpm(rpmRequest);
    setRotation(rotationInterrupt);
    vTaskDelay(pdMS_TO_TICKS(1));
}