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
#include "rpm.h"
#include "encoder.h"
#include "current.h"

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

void driveMotor(int pinDuty, int pinMotorRotation);

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

#if EDL_SELECTION == 0
    GPIO.func_out_sel_cfg[L1_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L2_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[L3_PIN].inv_sel = 1;
#else
    GPIO.func_out_sel_cfg[H1_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[H2_PIN].inv_sel = 1;
    GPIO.func_out_sel_cfg[H3_PIN].inv_sel = 1;
#endif

    xTaskCreatePinnedToCore(currentTask, "currentTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(calculateRpmTask, "calculateRpmTask", 2048, NULL, 1, NULL, 0);
    encoderInterrupt();
    driveMotor(0, 0);
    Serial.println("Setup Completed.");
}

void loop()
{
    static int motorRotation = 0;
    static int duty = 100;
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
        else if (gelen.toInt() > 0)
        {
            duty = gelen.toInt();
            if (duty > MAX_DUTY)
            {
                duty = MAX_DUTY;
            }
        }
        Serial.println(motorRotation);
        Serial.println(duty);
    }
    driveMotor(duty, motorRotation);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void driveMotor(int pinDuty, int pinMotorRotation)
{
    //float rpm = getRpm();
    float amper = getCurrent();
    //Serial.println("rpm : " + String(rpm));
    Serial.println("amper : " + String(amper));
    /*
    if (checkOverCurrent(amperLocal) == true)
    {
        errorState = OVER_CURRENT_ERROR;
        Serial.println("Overcurrent error : " + String(amperLocal));
    }
    */
    if (errorState != NO_ERROR)
    {
        pinDuty = 0;
        Serial.println("Error : " + String(errorState));
        errorState = NO_ERROR;
    }

    int highPwmDuty = 0;
    int lowPwmDuty = 0;

    if (pinMotorRotation == 0)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES + pinDuty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES - pinDuty;
    }
    if (pinMotorRotation == 1)
    {
        highPwmDuty = MOTOR_DRIVE_DUTY_RES - pinDuty;
        lowPwmDuty = MOTOR_DRIVE_DUTY_RES + pinDuty;
    }

    int encoderStepLocal = getEncoderStep();

    switch (encoderStepLocal)
    {
    case 101:
        ledcWrite(PWM_CHANNEL_1, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, highPwmDuty);
        break;
    case 100:
        ledcWrite(PWM_CHANNEL_1, highPwmDuty);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, MOTOR_DRIVE_DUTY_RES);
        break;
    case 110:
        ledcWrite(PWM_CHANNEL_1, highPwmDuty);
        ledcWrite(PWM_CHANNEL_2, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    case 10:
        ledcWrite(PWM_CHANNEL_1, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_2, highPwmDuty);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    case 11:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, highPwmDuty);
        ledcWrite(PWM_CHANNEL_3, MOTOR_DRIVE_DUTY_RES);
        break;
    case 1:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, MOTOR_DRIVE_DUTY_RES);
        ledcWrite(PWM_CHANNEL_3, highPwmDuty);
        break;
    default:
        ledcWrite(PWM_CHANNEL_1, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_2, lowPwmDuty);
        ledcWrite(PWM_CHANNEL_3, lowPwmDuty);
        break;
    }
}

