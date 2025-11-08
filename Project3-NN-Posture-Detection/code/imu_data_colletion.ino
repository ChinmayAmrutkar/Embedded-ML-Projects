/*
 * PROJECT 3 - IMU DATA COLLECTION SCRIPT
 *
 * This program reads all 6 axes from the onboard BMI270 
 * (accelerometer and gyroscope) and prints the data to the
 * Serial Monitor in CSV format.
 *
 * This data will be used to train our neural network.
 */

// Include the official Arduino library for the BMI270
#include <Arduino_BMI270_BMM150.h>

// Define the sampling period in milliseconds.
// 1000ms / 10ms = 100 Hz (100 samples per second).
const int SAMPLING_PERIOD_MS = 10;

void setup() {
  // Start Serial communication at 115200 baud (fast)
  Serial.begin(115200); 
  
  // Wait for the Serial Monitor to be opened.
  while (!Serial); 

  // Initialize the IMU sensor.
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1); // Halt program
  }

  // Print a status message to confirm setup is complete.
  Serial.println("IMU initialized. Starting data collection...");
  
  // Print the CSV header line.
  Serial.println("ax,ay,az,gx,gy,gz"); 
}

void loop() {
  // Declare floating-point variables to store the sensor readings.
  float ax, ay, az, gx, gy, gz;

  // Check if both accelerometer and gyroscope have new data available.
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    
    // Read the sensor data and store it in our variables.
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);

    // --- CSV Data Printing ---
    Serial.print(ax);
    Serial.print(",");
    Serial.print(ay);
    Serial.print(",");
    Serial.print(az);
    Serial.print(",");
    Serial.print(gx);
    Serial.print(",");
    Serial.print(gy);
    Serial.print(",");
    Serial.println(gz);
  }

  // Wait for the defined sampling period before the next loop.
  delay(SAMPLING_PERIOD_MS);
}
