# Embedded ML Projects Portfolio

This repository contains a collection of projects completed as part of an Embedded Machine Learning curriculum. Each project is contained within its own directory and includes source code, a detailed report, and a specific README file.

## Projects

#### ➡️ [Project 1: Reactive State Machine](https://github.com/ChinmayAmrutkar/Embedded-ML-Projects/tree/main/Project1-Reactive-StateMachine)
* **Description:** An implementation of a reactive state machine on an Arduino Nano 33 BLE Sense.
* **Key Concepts:** State management, hardware I/O (RGB LED), non-blocking timers (`millis()`), and serial communication for user input.
* **Status:** ✅ Complete

---

#### ➡️ [Project 2: Lying Posture Detection](https://github.com/ChinmayAmrutkar/Embedded-ML-Projects/tree/main/Project2-Posture-Detection)
* **Description:** A real-time system that detects a user's lying posture (supine, prone, side) using the onboard accelerometer.
* **Key Concepts:** IMU (BMI270) accelerometer, data-driven algorithm design, sense-process-act model, real-time systems, sampling frequency analysis.
* **Status:** ✅ Complete

---

#### ➡️ [Project 3: Robust Posture Classification](https://github.com/ChinmayAmrutkar/Embedded-ML-Projects/tree/main/Project3-NN-Posture-Detection)
* **Description:** An orientation-robust posture classifier built with a Neural Network. This model expands on Project 2 to classify 5 postures (`supine`, `prone`, `side`, `sitting`, `unknown`) regardless of sensor orientation.
* **Key Concepts:** Neural Networks (TensorFlow/Keras), 6-Axis IMU (Accel+Gyro), Robust Data Collection, Feature Engineering, Noise Injection, `softmax`, `EarlyStopping`.
* **Status:** ✅ Complete

---

#### ➡️ [Project 4: Sensor-Agnostic & Orientation-Insensitive Posture Detection](https://github.com/ChinmayAmrutkar/Embedded-ML-Projects/tree/main/Project4-Real-Time%20Sensor-Agnostic-Posture-Detection)
* **Description:** A complete, on-device system that classifies 5 postures with **99.12% accuracy**. This project's main features are its **sensor-agnostic** design (works with Accel, Gyro, or Mag) and **orientation-insensitive** design (works regardless of sensor rotation).
* **Key Concepts:** 1D-CNN, 9-Axis IMU, Sensor-Agnostic Normalization, On-Device Deployment (TFLite), `const` memory (SRAM vs. Flash), Robust Serial "Handshake" Protocol.
* **Status:** ✅ Complete
---
*(More projects will be added here as they are completed.)*
