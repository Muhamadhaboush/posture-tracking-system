# Posture Tracking System

This project is a wearable posture tracking system developed for the Microprocessors course. The system continuously monitors the user's posture and provides an audible warning when it detects an incorrect posture (e.g., slouching), prompting the user to correct their stance.

## Features

- **Multi-Sensor Data:** Precisely measures body movements and spinal curvature using two MPU6050 (Accelerometer and Gyroscope) sensors and one Flex sensor.
- **Wearable Design:** The sensors are sewn onto a flexible elastic band, making it comfortable to wear and suitable for daily use.
- **Instant Feedback:** When a posture outside the defined threshold values is detected, the system immediately provides an audible warning via the built-in Buzzer.

## Hardware Used

- 1x Arduino Uno (Main controller)
- 2x MPU6050 (Accelerometer and Gyroscope sensor)
- 1x Flex Sensor (Bend/Flex sensor)
- 1x Buzzer (For audible warnings)
- Elastic band (For the wearable apparatus)
- Jumper wires and power supply

## Project Images

*(Here are some images of the project)*
![Project Image 1](WhatsApp%20Image%202026-05-25%20at%2015.25.15%20(1).jpeg)
![Project Image 2](WhatsApp%20Image%202026-05-25%20at%2015.25.15.jpeg)

## Project Contents

- `posture_tracking_system_code/`: C++ source code written for Arduino Uno.
- `postur_tracking_system.fzz`: Fritzing circuit diagram.

## How It Works

1. The wearable band is placed on the user's back.
2. The MPU6050 sensors measure the tilt of the upper and lower back, while the Flex sensor detects how much the spine is bending.
3. The Arduino Uno continuously reads and analyzes data from these sensors.
4. If the sensor values indicate that the user is slouching or in an incorrect posture (when defined thresholds are exceeded), the Arduino triggers the Buzzer to warn the user.
5. The warning sound stops when the user returns to an upright posture.
