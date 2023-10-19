import serial
import matplotlib.pyplot as plt
from drawnow import drawnow
import numpy as np
from time import time, sleep
import threading

# Global variables for data storage
time_data = []  # For the x-axis (time)
data1 = []      # First variable (y-axis)
data2 = []      # Second variable (y-axis)
data_window = 4.0  # Time window to display in seconds

def make_fig():
    current_time = time_data[-1]
    time_window_start = current_time - data_window
    window_indices = [i for i, t in enumerate(time_data) if t >= time_window_start]
    plt.plot(np.array(time_data)[window_indices], np.array(data1)[window_indices], label='Variable 1')
    plt.plot(np.array(time_data)[window_indices], np.array(data2)[window_indices], label='Variable 2')
    plt.xlabel('Time')
    plt.ylabel('Values')
    plt.ylim(0, 50)
    plt.legend()

def console_thread():
    while True:
        user_input = input("Enter a number to send over serial (0-100, 'L' for left, 'R' for right): ")
        if user_input.lower() == 'l':
            ser.write(b"L\n")
        elif user_input.lower() == 'r':
            ser.write(b"R\n")
        else:
            try:
                number_to_send = float(user_input)
                ser.write(f"{number_to_send:.2f}\n".encode())
            except ValueError:
                print("Invalid input. Please enter a valid number (0-100), 'L', or 'R'.")

def graphics_thread():
    # Set up the plot
    plt.ion()
    plt.figure(figsize=(10, 6))

    while True:
        # Read and parse data from the serial port
        serial_data = ser.readline().decode().strip()
        data = serial_data.split()
        if len(data) == 2:
            try:
                data1_value = float(data[0])
                data2_value = float(data[1])
                current_time = time()
                time_data.append(current_time)
                data1.append(data1_value)
                data2.append(data2_value)

                # Remove data points that are outside the time window
                while time_data[0] < current_time - data_window:
                    time_data.pop(0)
                    data1.pop(0)
                    data2.pop(0)

                # Plot the data in real-time
                drawnow(make_fig)
            except ValueError:
                print("Invalid data format received from the serial port.")

if __name__ == "__main__":
    com_port = input("Enter the COM port (e.g., 'COM3' or '/dev/ttyUSB0'): ")
    try:
        ser = serial.Serial(com_port, 115200, timeout=1)
        print(f"Connected to {com_port}")
    except serial.SerialException:
        print(f"Failed to connect to {com_port}. Please check your COM port and try again.")
    console_thread = threading.Thread(target=console_thread)
    graphics_thread = threading.Thread(target=graphics_thread)
    console_thread.start()
    graphics_thread.start()
