/*
 * PROJECT 4 - 9-AXIS DATA COLLECTION SCRIPT
 *
 * This program reads all 9 axes from the onboard IMU
 * (BMI270 Accelerometer, BMI270 Gyroscope, BMM150 Magnetometer)
 * and prints the data to the Serial Monitor in CSV format at 50 Hz.
 */

#include "Arduino_BMI270_BMM150.h"

// Define our sampling period. 1000ms / 20ms = 50 Hz.
const int SAMPLING_PERIOD_MS = 20;

void setup() {
  Serial.begin(115200); // Use a fast baud rate
  while (!Serial);

  // Initialize the IMU (Accel, Gyro, Mag)
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("IMU initialized. Starting data collection...");
  // Print the 9-column CSV header
  Serial.println("ax,ay,az,gx,gy,gz,mx,my,mz");
}

void loop() {
  float ax, ay, az; // Accelerometer
  float gx, gy, gz; // Gyroscope
  float mx, my, mz; // Magnetometer

  // Check if all data is available
  if (IMU.accelerationAvailable() && 
      IMU.gyroscopeAvailable() && 
      IMU.magneticFieldAvailable()) 
  {
    // Read all 9 axes
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);
    IMU.readMagneticField(mx, my, mz);

    // Print the data as a single CSV line
    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.print(az); Serial.print(",");
    
    Serial.print(gx); Serial.print(",");
    Serial.print(gy); Serial.print(",");
    Serial.print(gz); Serial.print(",");
    
    Serial.print(mx); Serial.print(",");
    Serial.print(my); Serial.print(",");
    Serial.println(mz);
  }

  // Wait for the next sampling interval
  delay(SAMPLING_PERIOD_MS);
}
