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


void setup()
{
    Serial.begin(115200);

    pinMode(ENCODER_A_PIN, INPUT);
    pinMode(ENCODER_B_PIN, INPUT);
    pinMode(ENCODER_C_PIN, INPUT);
    pinMode(FAULT_PIN, INPUT);

    pinMode(H1_PIN, OUTPUT);
    pinMode(H2_PIN, OUTPUT);
    pinMode(H3_PIN, OUTPUT);
    pinMode(L1_PIN, OUTPUT);
    pinMode(L2_PIN, OUTPUT);
    pinMode(L3_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), encoderInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_C_PIN), encoderInterrupt, CHANGE);

    ledcSetup(PWM_CHANNEL_1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(PWM_CHANNEL_2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(PWM_CHANNEL_3, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

    ledcAttachPin(H1_PIN, PWM_CHANNEL_1);
    ledcAttachPin(L1_PIN, PWM_CHANNEL_1);
    ledcAttachPin(H2_PIN, PWM_CHANNEL_2);
    ledcAttachPin(L2_PIN, PWM_CHANNEL_2);
    ledcAttachPin(H3_PIN, PWM_CHANNEL_3);
    ledcAttachPin(L3_PIN, PWM_CHANNEL_3);

    GPIO.func_out_sel_cfg[L1_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L2_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L3_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[H1_PIN].inv_sel = 0;
    GPIO.func_out_sel_cfg[H2_PIN].inv_sel = 0;
    GPIO.func_out_sel_cfg[H3_PIN].inv_sel = 0;

    xTaskCreatePinnedToCore(currentTask, "currentTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(calculateRpmTask, "calculateRpmTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(motorTask, "motorTask", 2048, NULL, 1, NULL, 0);
    Serial.println("Setup Completed.");
}

static int motorRotation = 0;
static int rpm = 0;

void loop()
{
    if (Serial.available() > 0)
    {
        String gelen = Serial.readString();
        if (gelen == "R")
        {
            motorRotation = 1;
        }
        else if (gelen == "L")
        {
            motorRotation = 0;
        }
        else
        {
            rpm = gelen.toInt();
        }
    }
    setRpm(rpm);
    setRotation(motorRotation);
    vTaskDelay(pdMS_TO_TICKS(1));
}