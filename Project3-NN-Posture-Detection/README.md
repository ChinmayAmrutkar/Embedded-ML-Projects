# Project 3: Robust Posture Classification with a Neural Network

This repository contains all project deliverables for a real-time posture detection system built on the Arduino Nano 33 BLE Sense Rev2, as part of an embedded systems curriculum.

The system is trained on 6-axis IMU data (accelerometer + gyroscope) and can successfully classify five postures (`supine`, `prone`, `side`, `sitting`, `unknown`) with **100% test accuracy**, regardless of the sensor's orientation or simulated noise.

## Project Overview

This project's goal was to overcome the two main limitations of Project 2's `if/else` algorithm:
1.  **Limited Classes:** The old algorithm only knew 3 static postures.
2.  **Orientation-Dependent:** The old algorithm would fail if the sensor was worn upside-down.

This project solves both problems by using a data-centric machine learning approach. A neural network is trained on a "robust" dataset that includes multiple orientations and a new "unknown" motion class.

### Key Concepts
* **Neural Networks:** Building, training, and evaluating a Multi-Layer Perceptron (MLP) using TensorFlow/Keras.
* **6-Axis IMU Data:** Using both **Accelerometer** (for static gravity vectors) and **Gyroscope** (for dynamic motion) data as features.
* **Robust Data Collection:** Explicitly collecting data in multiple orientations (USB Up, USB Down, Left Side, Right Side) to build an orientation-independent model.
* **Feature Engineering:** Adding a dynamic `unknown` class and simulating real-world jitter by adding 10% Gaussian noise to the dataset.
* **ML Best Practices:** `StandardScaler` for feature scaling, `EarlyStopping` for regularization, and a 3-way `train/validation/test` split for robust evaluation.
* **Activation Function Analysis:** Comparing `ReLU`, `Tanh`, and `Sigmoid` to determine the most effective and efficient function for this problem (Spoiler: It's `ReLU`).


## Hardware and Software
* **Hardware:** Arduino Nano 33 BLE Sense Rev2
* **Arduino IDE:**
    * Libraries: `Arduino_BMI270_BMM150`
* **Python 3 / Jupyter Notebook:**
    * Libraries: `tensorflow`, `pandas`, `scikit-learn`, `matplotlib`, `seaborn`, `numpy`

## How to Run the Solution

This project is in two parts: data collection (on-device) and model training (offline).

### 1. Data Collection (Arduino)
1.  Open `src/data_collection.ino` in the Arduino IDE.
2.  Select the "Arduino Nano 33 BLE" board and the correct COM port.
3.  Upload the code to your board.
4.  Open the **Serial Monitor** and set the baud rate to **115200**.
5.  Hold the board in a specific posture (e.g., "Supine, USB Up").
6.  Let the data stream for 1-2 minutes.
7.  Copy all the text from the Serial Monitor and paste it into a new text file. **Ensure the header `ax,ay,az,gx,gy,gz` is the first line.**
8.  Save the file in the `/data/` folder with a descriptive name (e.g., `supine_usb_up_trial1.csv`).
9.  Repeat this process for all 5 classes in all their orientations (at least 20+ files total).

### 2. Model Training & Evaluation (Jupyter)
1.  Ensure all your collected `.csv` files are in the `/data/` folder.
2.  Open `src/Posture_Model_Code.ipynb` in Jupyter Lab or VS Code.
3.  Run all the cells from top to bottom.

The notebook will automatically:
1.  Load all 20+ data files.
2.  Add labels, inject 10% noise, and scale the data.
3.  Split the data into training, validation, and test sets.
4.  Train and compare the `ReLU`, `Tanh`, and `Sigmoid` models.
5.  Generate the training history plots.
6.  Select the best model (`ReLU`) and evaluate it on the unseen test set, printing the final Classification Report and Confusion Matrix.

## Final Results
The `ReLU` model was the clear winner, achieving the highest validation accuracy (**99.97%**) and converging **3x faster** than the other models (22 epochs).

When evaluated on the final, unseen test set, the model achieved **perfect 100% accuracy**, proving the data-centric design was a complete success.

### Test Set Classification Report
| | precision | recall | f1-score | support |
| :--- | ---: | ---: | ---: | ---: |
| supine | 1.00 | 1.00 | 1.00 | 2113 |
| prone | 1.00 | 1.00 | 1.00 | 2192 |
| side | 1.00 | 1.00 | 1.00 | 4347 |
| sitting | 1.00 | 1.00 | 1.00 | 2141 |
| unknown | 1.00 | 1.00 | 1.00 | 2404 |
| | | | | |
| accuracy | | | 1.00 | 13197 |
| macro avg | 1.00 | 1.00 | 1.00 | 13197 |
| weighted avg | 1.00 | 1.00 | 1.00 | 13197 |

---

*This project was developed as an academic exercise. The full, detailed analysis of the experimental design, results, and conclusions can be found in the `/report/Project_3.pdf` file.*
