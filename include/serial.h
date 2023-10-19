/**
 * @file serial.h
 * @brief Serial operations header file
 * @date 2023-10-19
 * @version 0.1
 * @author Halit Cetin (halitcetin@live.com)
 * @copyright Copyright (c) 2023
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#define MAX_INPUT_LENGTH 128

/**
 * @brief Task that handles serial input and output.
 *
 * This function is the main task that handles serial input and output.
 * It reads user input from the serial port, writes it to the input queue, and
 * reads from the output queue to print messages to the console.
 *
 * @param param Task parameter (not used in this context).
 */
extern void serialTask(void *param);

/**
 * @brief Read a line from the input queue (blocking).
 *
 * This function reads a line of text from the input queue, blocking until
 * a line is available.
 *
 * @return A String containing the received line of text.
 */
extern String serialRead();

/**
 * @brief Write a message to the output queue.
 *
 * This function writes a message to the output queue, which is then printed to
 * the serial console by the serialTask.
 *
 * @param message The message to be written to the output queue.
 */
extern void serialWrite(const char *message);

/**
 * @brief Write a message to the output queue.
 *
 * This function writes a message to the output queue, which is then printed to
 * the serial console by the serialTask.
 *
 * @param message The message to be written to the output queue.
 */
extern void serialWrite(String message);

#endif
