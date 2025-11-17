import serial
import time
import sys

# --- Configuration ---
SERIAL_PORT = "COM9"   
BAUD_RATE = 115200

# Use a SHORT per-line timeout so we poll often,
# and manage a LONG overall deadline ourselves.
PER_LINE_TIMEOUT_SECS = 0.2
OVERALL_DEADLINE_SECS = 12.0  # enough for 2s capture + inference + prints
WRITE_TIMEOUT_SECS = 2
# ---------------------

CLASS_NAMES = ['supine', 'prone', 'side', 'sitting', 'unknown']

def wait_for_ready_banner(ser, timeout=10):
    end = time.time() + timeout
    ser.timeout = PER_LINE_TIMEOUT_SECS
    while time.time() < end:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        if not line:
            continue
        print(f"[Arduino]: {line}")
        if "Ready" in line or "READY" in line:
            return True
    return False

def drain_serial(ser):
    ser.timeout = 0.05
    while ser.readline():
        pass

def read_until_ready(ser, overall_deadline=OVERALL_DEADLINE_SECS):
    """Keep reading lines until we see READY or we run past the overall deadline.
       Return (prediction_digit_or_empty, saw_ready_bool)."""
    ser.timeout = PER_LINE_TIMEOUT_SECS
    deadline = time.time() + overall_deadline
    prediction = ""

    while time.time() < deadline:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        if not line:
            # No line this tick; keep waiting (Arduino is collecting/inferencing)
            continue

        print(f"  [Arduino]: {line}")

        # Capture single-digit prediction 1..5 if it appears at any time
        if len(line) == 1 and line.isdigit():
            prediction = line

        # When READY appears, we are done
        if "READY" in line or "Ready for next command" in line:
            return prediction, True

    # Deadline exceeded
    return prediction, False

def main():
    if not SERIAL_PORT:
        print("Set SERIAL_PORT.")
        sys.exit(1)

    print(f"Connecting to {SERIAL_PORT} @ {BAUD_RATE}...")
    try:
        ser = serial.Serial(
            SERIAL_PORT,
            BAUD_RATE,
            timeout=PER_LINE_TIMEOUT_SECS,
            write_timeout=WRITE_TIMEOUT_SECS,
        )
    except serial.SerialException as e:
        print("Serial open failed:", e)
        sys.exit(1)

    time.sleep(2)  # allow board reset
    print("Waiting for Ready...")
    if not wait_for_ready_banner(ser, timeout=12):
        print("Warning: no initial Ready seen; proceeding anyway.")

    SENSOR_TOKENS = {'1': "1\n", '2': "2\n", '3': "3\n"}

    try:
        while True:
            print("\n--- Real-Time Posture Prediction ---")
            print("[1] Accelerometer    [2] Gyroscope    [3] Magnetometer    [q] Quit")
            choice = input("Enter choice: ").strip().lower()
            if choice == 'q':
                break
            if choice not in ('1', '2', '3'):
                print("Invalid choice.")
                continue

            # Send a clean single-line command
            token = SENSOR_TOKENS[choice]
            print(f"Sending: {token.strip()}")
            try:
                drain_serial(ser)                 # drop any old prints
                ser.write(token.encode('utf-8'))  # send one clean command
                ser.flush()
            except serial.SerialTimeoutException:
                print("Write timeout.")
                continue

            # Read until READY (or overall deadline)
            prediction_str, ok = read_until_ready(ser, overall_deadline=OVERALL_DEADLINE_SECS)

            if not ok:
                print(">>> Error: Operation timed out before READY.")
                continue

            if prediction_str:
                idx = int(prediction_str) - 1
                if 0 <= idx < len(CLASS_NAMES):
                    print(f">>> Prediction: {CLASS_NAMES[idx].upper()} ({idx+1})")
                else:
                    print(f">>> Error: out-of-range label '{prediction_str}'")
            else:
                print(">>> Error: Did not receive a valid prediction.")
    finally:
        try:
            ser.close()
        except Exception:
            pass
        print("Serial port closed.")

if __name__ == "__main__":
    main()
