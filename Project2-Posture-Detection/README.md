# Project 2: Real-Time Lying Posture Detection

This repository contains all project deliverables for a real-time posture detection system built on the Arduino Nano 33 BLE Sense Rev2, as part of an embedded systems curriculum.

<img width="473" height="166" alt="image" src="https://github.com/user-attachments/assets/2c8be69f-1184-4172-b4a3-46bcdab7fcb5" />


## Project Overview

The goal of this project was to design, implement, and document a complete embedded system that follows a "sense-process-act" model.

1.  **Sense:** The system uses the onboard BMI270 accelerometer to read the device's orientation relative to the constant 1G force of gravity.
2.  **Process:** It processes this data using a data-driven, "ad-hoc" algorithm to classify the user's lying posture into one of three categories: **Supine** (on back), **Prone** (on stomach), or **Side**.
3.  **Act:** It provides real-time visual feedback by blinking the **onboard Red LED**.

This project emphasizes the importance of data collection, sensor-data analysis, and efficient real-time implementation.

## Functionality & Features

* **Real-Time Classification:** Detects three static postures (Supine, Prone, Side) and a fourth "Undefined" state.
* **Data-Driven Algorithm:** The classification logic is not arbitrary. It is based on a simple threshold algorithm developed by collecting and analyzing "ground truth" sensor data for each posture.
* **Sensor Selection:** The algorithm deliberately uses **only the accelerometer** to determine static orientation (gravity vector) and excludes the gyroscope (rotational velocity) for greater efficiency and stability.
* **Onboard Visual Feedback:** The built-in common-anode **Red LED** (pin 22) provides clear output:
    * **1 Blink:** Supine
    * **2 Blinks:** Prone
    * **3 Blinks:** Side
    * **LED OFF:** Undefined
* **Dual-Frequency Design:** The project includes two main scripts, demonstrating a key embedded systems concept:
    * `data_collection.ino`: A high-frequency (**100Hz**) script for capturing high-fidelity data for analysis.
    * `real_time_posture_detection.ino`: A low-frequency (**2Hz**) script for efficient, real-time deployment.

## Algorithm & Data

The core of the algorithm comes from a direct observation of the data collected. By plotting the accelerometer values for each posture, a clear pattern emerges: gravity is isolated to a single axis.

| Posture | `az` (Z-axis) Reading | `ay` (Y-axis) Reading |
| :--- | :---: | :---: |
| **Supine** | `~ +1.0` | `~ 0.0` |
| **Prone** | `~ -1.0` | `~ 0.0` |
| **Side** | `~ 0.0` | `~ ±1.0` |

This data directly inspired the simple `if...else if` algorithm, which uses a threshold of `0.8` to provide robustness against slight tilts.
Supine Data Plot:
<img width="973" height="431" alt="image" src="https://github.com/user-attachments/assets/3daa5986-b5d8-4e48-a212-0282002c5cf0" />

Prone Data Plot:
<img width="979" height="438" alt="image" src="https://github.com/user-attachments/assets/6698584f-300d-4d28-b81e-57bbe0be46ba" />

Left-Side Data Plot:
<img width="975" height="439" alt="image" src="https://github.com/user-attachments/assets/411a1a0f-9cd2-4004-a277-2ee619f6b04a" />

Right-Side Data Plot:
<img width="975" height="436" alt="image" src="https://github.com/user-attachments/assets/b3a15897-e18d-41e1-be53-9b8e42c3d473" />

## Hardware and Software Requirements

### Hardware
* Arduino Nano 33 BLE Sense Rev2

### Software
* Arduino IDE (version 2.0 or later)
* Serial Monitor (for data collection)
* Libraries: `Arduino_BMI270_BMM150`

## Setup and Demonstration

This project has two distinct phases, each with its own code.

### Part A: Data Collection (Replicating the Experiment)
1.  **Open Code:** Open `src/data_collection.ino` in the Arduino IDE.
2.  **Upload:** Upload the code to your board.
3.  **Open Serial Monitor:** Open the Serial Monitor and set the baud rate to **115200**.
4.  **Collect:** Hold the board in a static posture (e.g., flat on your desk, face up for "Supine"). Copy the streaming CSV data and paste it into a `.csv` file.

### Part B: Real-Time Posture Detection (Running the Demo)
1.  **Open Code:** Open `src/real_time_posture_detection.ino` in the Arduino IDE.
2.  **Upload:** Upload the code to your board.
3.  **Test:** Move the board to mimic the different postures and observe the onboard Red LED.
    * Face up -> LED blinks once.
    * Face down -> LED blinks twice.
    * On its edge -> LED blinks three times.
    * At an angle -> LED stays off.

## Demonstration Video

A short video (under 2 minutes) showing the final system working correctly can be viewed here:

[Demo Video](https://youtu.be/iWJLIzVa-z8?si=e3YuSY6yMhBOzFya)

---
*This project was developed as an academic exercise. The accompanying report in the `/report` directory provides a detailed breakdown of the system design, experimental procedure, algorithm, and results.*
