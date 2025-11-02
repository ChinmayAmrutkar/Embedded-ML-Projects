/*
  This program reads accelerometer and gyroscope data from the
  onboard BMI270 IMU and prints it to the Serial Monitor
  in a CSV (Comma-Separated Value) format.
  This data will be used to design our posture detection algorithm.
*/

#include <Arduino_BMI270_BMM150.h> // Include the library for the IMU

// Define the sampling period in milliseconds.
// 1000ms / 10ms = 100 Hz (100 samples per second).
// balance between responsiveness and not flooding the serial port.
const int SAMPLING_PERIOD_MS = 10;

void setup() {
  Serial.begin(115200); // Use a fast baud rate for data transfer
  while (!Serial); // Wait for serial monitor to open

  // Initialize the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1); // Halt the program
  }

  Serial.println("IMU initialized. Starting data collection...");
  Serial.println("ax,ay,az,gx,gy,gz"); // Print CSV header
}

void loop() {
  float ax, ay, az, gx, gy, gz;

  // Check if both accelerometer and gyroscope data are available
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    // Read the sensor data
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);

    // Print the data as a single CSV line
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

  // Wait for the next sampling interval
  delay(SAMPLING_PERIOD_MS);
}
