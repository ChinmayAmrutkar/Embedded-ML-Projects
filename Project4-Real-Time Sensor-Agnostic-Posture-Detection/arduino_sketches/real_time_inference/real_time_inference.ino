/*
 * REAL-TIME POSTURE DETECTION (strict command-driven, timeout-safe)
 */

#include <Arduino.h>

// --- TFLite Core Headers ---
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>

// --- Project-Specific Headers ---
#include "model.h"
#include "Arduino_BMI270_BMM150.h" 

// --- TFLite Globals ---
tflite::MicroErrorReporter tflErrorReporter;
tflite::AllOpsResolver    tflOpsResolver;
const tflite::Model*      tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor*             tflInputTensor = nullptr;
TfLiteTensor*             tflOutputTensor = nullptr;

// --- Memory Arena ---
constexpr int kTensorArenaSize = 15 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// --- Project Constants ---
const int SAMPLING_FREQUENCY_HZ = 50;
const int SAMPLING_PERIOD_MS    = 1000 / SAMPLING_FREQUENCY_HZ; // 20ms
const int WINDOW_SIZE           = 100; // 2 seconds @ 50Hz
const int NUM_CLASSES           = 5;

// Buffer to hold one window of data (100 samples, 3 axes)
float window_buffer[WINDOW_SIZE][3];

// --- Normalization Constants from normalization_params.json file ---
const float ACCEL_MEAN[] = { 0.001600526, -0.017278421, -0.021210526 };
const float ACCEL_STD[]  = { 0.525893116, 0.519990200, 0.707250449 };

const float GYRO_MEAN[]  = { -1.301315789, 0.011415263, 0.051266315 };
const float GYRO_STD[]   = { 48.513148771, 56.630698703, 40.677223375 };

const float MAG_MEAN[]   = { -30.665157894, 33.622736842, -5.371315789 };
const float MAG_STD[]    = { 100.036291765, 69.151219412, 82.474813816 };

// --- Protocol helpers ---
enum SensorSel { SEL_NONE = 0, SEL_ACCEL = 1, SEL_GYRO = 2, SEL_MAG = 3 };

static inline void clear_serial_rx() {
  while (Serial.available() > 0) (void)Serial.read();
}

static inline void clear_window_buffer() {
  for (int i = 0; i < WINDOW_SIZE; ++i) {
    window_buffer[i][0] = 0.f;
    window_buffer[i][1] = 0.f;
    window_buffer[i][2] = 0.f;
  }
}

static inline SensorSel parse_command_line(const String& cmd) {
  if (cmd == "1" || cmd == "SENSOR:ACCEL") return SEL_ACCEL;
  if (cmd == "2" || cmd == "SENSOR:GYRO")  return SEL_GYRO;
  if (cmd == "3" || cmd == "SENSOR:MAG")   return SEL_MAG;
  return SEL_NONE;
}

static void echo_cmd(SensorSel sel) {
  Serial.print("CMD:");
  if (sel == SEL_ACCEL)      Serial.println("ACCEL");
  else if (sel == SEL_GYRO)  Serial.println("GYRO");
  else if (sel == SEL_MAG)   Serial.println("MAG");
  else                       Serial.println("UNKNOWN");
}

static bool read_one_sample(SensorSel sel, float &x, float &y, float &z) {
  // Blocking, waits until the selected sensor reports new data
  if (sel == SEL_ACCEL) {
    while (!IMU.accelerationAvailable()) {}
    IMU.readAcceleration(x, y, z);
    return true;

  } else if (sel == SEL_GYRO) {
    while (!IMU.gyroscopeAvailable()) {}
    IMU.readGyroscope(x, y, z);
    return true;

  } else { // SEL_MAG
    while (!IMU.magneticFieldAvailable()) {}
    IMU.readMagneticField(x, y, z);
    return true;
  }
}
static bool collect_window(SensorSel sel) {
  Serial.print("Collecting ");
  if (sel == SEL_ACCEL)      Serial.print("Accel");
  else if (sel == SEL_GYRO)  Serial.print("Gyro");
  else if (sel == SEL_MAG)   Serial.print("Mag");
  else return false;
  Serial.println(" data for 2 seconds...");

  bool warned = false;
  unsigned long next_tick = millis(); // pacing anchor

  for (int i = 0; i < WINDOW_SIZE; i++) {
    // Pace: target every 20 ms; catch up if late
    next_tick += SAMPLING_PERIOD_MS;

    float sx = 0, sy = 0, sz = 0;
    bool fresh = read_one_sample(sel, sx, sy, sz);
    if (!fresh && !warned) {
      Serial.println("Note: sensor timeout on some samples (filling zeros).");
      warned = true;
    }
    window_buffer[i][0] = sx;
    window_buffer[i][1] = sy;
    window_buffer[i][2] = sz;

    long now = (long)millis();
    long wait_ms = (long)next_tick - now;
    if (wait_ms > 0) delay((unsigned long)wait_ms);
  }

  Serial.println("Collection complete. Normalizing...");
  return true;
}

static void normalize_fill_input(SensorSel sel) {
  for (int i = 0; i < WINDOW_SIZE; i++) {
    for (int j = 0; j < 3; j++) {
      float val;
      if (sel == SEL_ACCEL)      val = (window_buffer[i][j] - ACCEL_MEAN[j]) / ACCEL_STD[j];
      else if (sel == SEL_GYRO)  val = (window_buffer[i][j] - GYRO_MEAN[j])  / GYRO_STD[j];
      else /* SEL_MAG */         val = (window_buffer[i][j] - MAG_MEAN[j])   / MAG_STD[j];
      tflInputTensor->data.f[i * 3 + j] = val;   // input is [1,100,3] flattened
    }
  }
}

static void run_once(SensorSel sel) {
  // Start from a clean slate each cycle
  clear_serial_rx();          // ignore any typed-ahead input
  clear_window_buffer();      // clear previous samples

  echo_cmd(sel);

  if (!collect_window(sel)) {
    Serial.println("Collection failed");
    Serial.println("READY");
    return;
  }

  Serial.println("Running inference...");
  normalize_fill_input(sel);

  if (tflInterpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke failed");
    Serial.println("READY");
    return;
  }

  // Argmax over NUM_CLASSES
  int   predicted_label = 0;
  float max_score       = tflOutputTensor->data.f[0];
  for (int i = 1; i < NUM_CLASSES; i++) {
    float s = tflOutputTensor->data.f[i];
    if (s > max_score) { max_score = s; predicted_label = i; }
  }

  // Send 1..5 to host (compat with Python)
  Serial.println(predicted_label + 1);
  Serial.println("READY");
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) {}

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1) {}
  }

  // --- TFLite setup ---
  tflModel = tflite::GetModel(model_tflite);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1) {}
  }

  tflInterpreter = new tflite::MicroInterpreter(
    tflModel, tflOpsResolver, tensor_arena, kTensorArenaSize, &tflErrorReporter
  );
  if (tflInterpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("AllocateTensors() failed");
    while (1) {}
  }

  tflInputTensor  = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  clear_window_buffer();
  clear_serial_rx();

  Serial.println("Ready");
  Serial.println("Send 1 (ACCEL), 2 (GYRO), or 3 (MAG).");
}

void loop() {
  // STRICT IDLE → RUN-ONCE → IDLE behavior
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) return;

    SensorSel sel = parse_command_line(cmd);
    if (sel == SEL_NONE) {
      Serial.println("Unknown command. Use 1 | 2 | 3.");
      Serial.println("READY");
      return;
    }

    // From here until we print READY again, we do not look at Serial.
    run_once(sel);
  }
}
