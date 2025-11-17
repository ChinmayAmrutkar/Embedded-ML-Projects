# Project 4: Sensor-Agnostic and Orientation-Insensitive Posture Detection

This repository contains all project deliverables for a real-time, on-device posture classification system built on the Arduino Nano 33 BLE, as part of the CEN 598 Embedded ML curriculum.

The system is trained on 9-axis IMU data (accelerometer, gyroscope, and magnetometer) and can successfully classify five postures (`supine`, `prone`, `side`, `sitting`, `unknown`) with **99.12% test accuracy**.

Its core features are its **sensor-agnostic** design (it can use any of the 3 sensors for a valid prediction) and its **orientation-insensitive** design (it works regardless of the sensor's rotation on the user's chest).

## Project Overview

This project's goal was to build a complete, robust, and deployable machine learning system. This required solving two primary challenges:
1.  **Sensor-Agnosticism:** The system must not be tied to a single sensor. It should be able to make a prediction using data from the accelerometer, gyroscope, *or* magnetometer.
2.  **Orientation-Insensitivity:** The system must be robust to how the user wears it. It must correctly identify postures (e.g., "side") even if the sensor is "upside-down" (USB-down).

This project solves both problems by using a data-centric machine learning approach. A single 1D-CNN is trained on a "robust" dataset that includes normalized data from all three sensors, collected in multiple orientations.

### Key Concepts
* **1D Convolutional Neural Network:** Building, training, and evaluating a 1D-CNN, which is ideal for time-series sensor data.
* **9-Axis IMU Data:** Using all three 3-axis sensors (Accelerometer, Gyroscope, Magnetometer) as potential feature inputs.
* **Sensor-Agnostic Design:** Training a single model on a *combined* dataset of independently normalized data from all 3 sensors.
* **Orientation-Insensitive Design:** Explicitly collecting training data in multiple orientations (USB Up/Down, and `left_side`/`right_side`) to build a robust model.
* **On-Device Deployment:** Converting the final Keras model to a C-array (`model.h`) using TensorFlow Lite and deploying it to the Arduino's Flash memory.
* **Robust Serial Protocol:** Implementing a "handshake" protocol (`SENSOR:TOKEN` / `CMD:TOKEN` / `READY`) between the Python host and Arduino to ensure stable, desync-proof communication.

## Hardware and Software
* **Hardware:** Arduino Nano 33 BLE
* **Arduino IDE:**
    * Libraries: `TensorflowLite_Library_export` (course-provided), `Arduino_BMI270_BMM150`
* **Python 3 / Google Colab:**
    * Libraries: `tensorflow`, `pandas`, `scikit-learn`, `matplotlib`, `seaborn`, `numpy`
* **Host (Laptop):**
    * Libraries: `pyserial`

## Repository Structure
```
├── README.md
│
├── notebook/
│   └── posture_project.ipynb   (The Colab notebook for data processing & model training)
│
├── data/
│   └── ... (All raw .csv data files used for training)
│
├── arduino_sketches/
│   ├── 1_data_collector/
│   │   └── data_collector.ino  (Arduino sketch for collecting new data)
│   └── 2_real_time_inference/
│       ├── real_time_inference.ino (The final deployment sketch)
│       └── model.h                 (The TFLite model as a C-array)
│
├── host_script/
│   └── base_station.py         (The Python script to run the demo)
│
└── report/
    ├── Project4_Report.pdf     (The final project report)
    ├── training_history.png
    └── confusion_matrix.png
```
## How to Implement

This project is in three parts: data collection, model training (offline), and real-time inference (on-device).

### 1. Data Collection (Arduino + Python)
1.  Open `arduino_sketches/data_collector/data_collector.ino` in the Arduino IDE and upload it.
2.  On the host computer copy paste the data from Arduino's Serial Monitor to .csv
3.  Save it in a descriptive filename (e.g., `supine_usb_up_trial1.csv`).
4.  Perform the posture for 45-60 seconds. 
5.  Repeat this process for all 5 classes in all their orientations, placing the `.csv` files in the `data/` folder.

### 2. Model Training & Conversion (Google Colab)
1.  Create a `data` folder in your Colab environment and upload all your collected `.csv` files.
2.  Open `notebook/posture_project.ipynb` in Google Colab.
3.  Run all the cells from top to bottom.

The notebook will automatically:
1.  Load all data files from the `data/` folder.
2.  Process labels, balance the classes (via undersampling), and normalize all 3 sensor types independently.
3.  Save the 18 normalization values to `normalization_params.json`.
4.  Train and evaluate the 1D-CNN model.
5.  Generate the training history and confusion matrix plots.
6.  Convert the final model to `model.h` and package all project files into `Arduino_Project_Files.zip` for download.

### 3. Real-Time Inference (Arduino + Python)
1.  Open `arduino_sketches/real_time_inference/real_time_inference.ino` in the Arduino IDE.
2.  Add the `model.h` file (from your `.zip`) to the same sketch folder.
3.  **Crucially:** Manually edit `model.h` and add `const` to the `model_tflite[]` array and `model_tflite_len` variables. This fixes the SRAM overflow (linker) error by storing the model in Flash.
4.  Open `normalization_params.json` and copy the 18 `mean`/`std` values into the corresponding constant arrays at the top of the `.ino` sketch.
5.  Upload the completed sketch to your Arduino.
6.  On your host computer, open `host_script/base_station.py` and set the `SERIAL_PORT` variable (e.g., `"COM9"`) to match your Arduino.
7.  Run the script: `python host_script/base_station.py`.
8.  Follow the menu prompts (1, 2, or 3) to see the live, sensor-agnostic predictions.

## Final Results
The 1D-CNN model was a complete success. After tuning the training parameters (`epochs=100`, `patience=10`), the model achieved **near-perfect 99.12% accuracy** on the unseen test set.

The robust data collection and normalization strategy was highly effective, with the model perfectly distinguishing `supine` and `prone`, and only showing trivial confusion on `side` and `sitting`.

### Test Set Classification Report
| | precision | recall | f1-score | support |
| :--- | ---: | ---: | ---: | ---: |
| **supine** | 1.00 | 1.00 | 1.00 | 23 |
| **prone** | 1.00 | 1.00 | 1.00 | 22 |
| **side** | 1.00 | 0.96 | 0.98 | 23 |
| **sitting** | 0.96 | 1.00 | 0.98 | 23 |
| **unknown** | 1.00 | 1.00 | 1.00 | 23 |
| | | | | |
| **accuracy** | | | **0.99** | **114** |
| **macro avg** | 0.99 | 0.99 | 0.99 | 114 |
| **weighted avg** | 0.99 | 0.99 | 0.99 | 114 |

## Demonstration Video
A short video showing the final system working correctly can be viewed here: <br>
[Demo Video](https://youtu.be/sGtFIbxpJlU)

---

*This project was developed as an academic exercise. The full, detailed analysis of the experimental design, results, and conclusions can be found in the `/report/Project4_Report.pdf` file.*
