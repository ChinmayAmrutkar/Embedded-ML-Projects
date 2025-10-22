# Project 1: Reactive State Machine on Arduino Nano 33 BLE Sense

This repository contains the code, report, and documentation for a reactive state machine implemented on an Arduino Nano 33 BLE Sense, as part of an embedded systems curriculum.

## Project Overview

The goal of this project was to implement a given state transition diagram on a microcontroller. The system uses the onboard RGB LED to visually represent the current state and a "soft switch" (via the Serial Monitor) to handle user input. The project demonstrates core embedded systems concepts, including state management, hardware I/O, and non-blocking timers.

### State Machine Diagram

The behavior of the system is governed by the following state diagram:
<img width="542" height="256" alt="image" src="https://github.com/user-attachments/assets/dbebd36c-eb20-422a-8a6e-2d56379a7bc6" />


## Functionality & Features

* **State Representation:** The system's four states (Dark, Red, Blue, Green) are represented by the color of the built-in RGB LED.
* **User Input:** A "soft switch" is implemented using the Arduino IDE's Serial Monitor. Sending the character 'S' triggers a state transition.
* **Non-Blocking Timers:** Time-based transitions are handled using the `millis()` function, ensuring the system remains fully responsive to user input even during timeout periods.
* **Debug-Friendly:** The serial interface provides real-time feedback on state changes and their triggers (e.g., "Switch: RED -> BLUE" or "Timeout: GREEN -> BLUE").
* **Self-Contained:** The project runs entirely on the Arduino Nano 33 BLE Sense without any external hardware or shields.

## Hardware and Software Requirements

### Hardware

* Arduino Nano 33 BLE Sense

### Software

* Arduino IDE (version 2.0 or later recommended)

## Setup and Demonstration

1.  **Connect Hardware:** Connect the Arduino Nano 33 BLE Sense to your computer via USB.
2.  **Open Code:** Open the `src/StateMachine_Serial.ino` file in the Arduino IDE.
3.  **Upload:** Select the correct board and port, then upload the code.
4.  **Open Serial Monitor:** Open the Serial Monitor (Ctrl+Shift+M) and set the baud rate to **115200**.
5.  **Interact:** Type `S` into the input field and press Enter to simulate a switch press and cycle through the states. Observe the LED changes and the status messages in the monitor.

## Demonstration Video

A short video demonstrating all 7 state transitions can be viewed here:

https://youtu.be/VLAV8kEXP-E

---
*This project was developed as an academic exercise. The accompanying report in the `/report` directory provides a detailed breakdown of the design, implementation, and lessons learned.*
