/*
 * PROJECT 2 - REAL-TIME POSTURE DETECTION
 * This program uses the onboard BMI270 accelerometer to
 * determine a user's lying posture. It implements an
 * "ad-hoc" algorithm developed by observing collected sensor data.
 * The system indicates the detected posture by blinking an
 * LED connected to pin 22 in specific patterns.
 */

// Include the official Arduino library for the BMI270.
#include "Arduino_BMI270_BMM150.h"

// Define the digital pin number for the LED.
#define LED_PIN 22 

// Define the logic levels for our common ANODE LED.
#define LED_ON  LOW
#define LED_OFF HIGH

// Define the posture detection threshold.
// Based on our data, the sensor reads ~1.0G when aligned
// with gravity. We use 0.8 to allow for some tilt/error
// without causing a false negative.
const float THRESHOLD = 0.8;

// Define the "meaningful frequency" of our algorithm.
// The main loop will pause for this duration.
// This sets our detection rate (1000ms / 500ms = 2 Hz)
// and gives the LED time to blink.
const int LOOP_DELAY = 500; 

void setup() {

  Serial.begin(115200);
   while (!Serial);

  // Initialize the IMU sensor.
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1); // Halt program
  }

  // Configure the LED_PIN (Pin 22) as an OUTPUT.
  pinMode(LED_PIN, OUTPUT);
  
  // Set the initial state of the LED to OFF.
  digitalWrite(LED_PIN, LED_OFF); 
  
  // Print status messages to the Serial Monitor.
  Serial.println("Posture detection system active.");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
}

void loop() {
  // Declare variables for sensor data.
  // Note: We only need x, y, and z (accelerometer).
  float x, y, z;

  // Check if new acceleration data is available from the IMU.
  if (IMU.accelerationAvailable()) {
    
    // Read the acceleration data
    IMU.readAcceleration(x, y, z);
    
    // This series of if/else if statements classifies the
    // posture based on the data analysis

    if (z > THRESHOLD) {
      // POSTURE: Supine (On Back)
      // Gravity is pulling along the positive Z-axis.
      Serial.println("Supine (Back)");
      blinkLED(1); // Blink once
      
    } else if (z < -THRESHOLD) {
      // POSTURE: Prone (On Stomach)
      // Gravity is pulling along the negative Z-axis.
      Serial.println("Prone (Stomach)");
      blinkLED(2); // Blink twice
      
    } else if (y > THRESHOLD || y < -THRESHOLD) {
      // POSTURE: Side (Left or Right)
      // Gravity is pulling along the positive Y-axis (Left)
      // OR negative Y-axis (Right).
      Serial.println("Side");
      blinkLED(3); // Blink three times
      
    } else {
      // POSTURE: Undefined
      // None of the primary axes are aligned with gravity.
      // (e.g., sitting up, transitioning between postures)
      Serial.println("Undefined");
      digitalWrite(LED_PIN, LED_OFF); // Keep LED off
    }

    // Wait for our defined loop delay. This sets the
    // "meaningful frequency" of our system to 2 Hz.
    delay(LOOP_DELAY); 
  }
}

/**
 * Helper function to blink the LED a specific number of times.
 * The number of blinks desired (e.g., 1, 2, or 3).
 */
void blinkLED(int times) {
  // Loop 'times' number of times
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LED_ON);  // Turn LED on
    delay(150); // Keep LED on for 150ms
    digitalWrite(LED_PIN, LED_OFF); // Turn LED off
    delay(150); // Keep LED off for 150ms
  }
}
