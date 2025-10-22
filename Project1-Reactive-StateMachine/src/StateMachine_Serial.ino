// State Definitions
// Using simple numbers to represent the system states.
#define STATE_DARK  0
#define STATE_RED   1
#define STATE_BLUE  2
#define STATE_GREEN 3

// LED Pin Definitions
// Physical pins for the built-in RGB LED on the Nano 33 BLE Sense.
const int RED_PIN = 22;
const int GREEN_PIN = 23;
const int BLUE_PIN = 24;

// Variable to hold the current state, starting with DARK (0).
int currentState = STATE_DARK;

// Variable to store the last time a state with a timer was entered.
unsigned long stateEnterTime = 0;

void setup() {
  // Initialize serial communication at 115200 baud for the soft switch and debugging.
  Serial.begin(115200);
  
  // Set the LED pins as outputs.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Start in the DARK state.
  setLedColor(STATE_DARK);
  Serial.println("System ready. Send 'S' to press the switch.");
}

void loop() {
  // Part 1: Check for the Switch Press ('S' key as a Serian Input)
  // This block checks if there is any data available to read from the serial port
  if (Serial.available() > 0) {
    char input = Serial.read();   // Read the incoming character.
    // Check if the input character is 'S' or 's' to trigger the switch.
    if (input == 'S' || input == 's') {
      // Use a switch statement to determine the next state based on the current state.
      switch (currentState) {
        
        // CASE: The system is currently in the DARK state.
        case STATE_DARK:
          Serial.println("Switch: DARK -> RED");
          // Transition to the RED state upon switch press.
          currentState = STATE_RED;
          break;

        // CASE: The system is currently in the RED state.
        case STATE_RED:
          Serial.println("Switch: RED -> BLUE");
          // Transition to the BLUE state upon switch press.
          currentState = STATE_BLUE;
          break;

        // CASE: The system is currently in the BLUE state.
        case STATE_BLUE:
          Serial.println("Switch: BLUE -> GREEN");
          // Transition to the GREEN state upon switch press.
          currentState = STATE_GREEN;
          break;

        // CASE: The system is currently in the GREEN state.
        case STATE_GREEN:
          Serial.println("Switch: GREEN -> DARK");
          // Transition back to the DARK state upon switch press, completing the cycle.
          currentState = STATE_DARK;
          break;
      }
      // Every time the state changes, update the LED and reset our timer.
      setLedColor(currentState);
      stateEnterTime = millis();
    }
  }

  // Part 2: Check for Timeouts
  // This part runs continuously, checking if enough time has passed.
  switch (currentState) {
    // CASE: The system is currently in the RED state.
    case STATE_RED:
      // Check if 5000ms (5 seconds) have passed since entering this RED state.
      if (millis() - stateEnterTime >= 5000) {
        Serial.println("Timeout: RED -> DARK");
        // If the timeout is reached, transition back to the DARK state.
        currentState = STATE_DARK;
        setLedColor(currentState);
        stateEnterTime = millis(); // Reset timer for the new state.
      }
      break;

    // CASE: The system is currently in the BLUE state.
    case STATE_BLUE:
      // Check if 4000ms (4 seconds) have passed since entering this BLUE state.
      if (millis() - stateEnterTime >= 4000) {
        Serial.println("Timeout: BLUE -> RED");
        // If the timeout is reached, transition back to the RED state.
        currentState = STATE_RED;
        setLedColor(currentState);
        stateEnterTime = millis(); // Reset timer for the new state.
      }
      break;

    // CASE: The system is currently in the GREEN state.
    case STATE_GREEN:
      // Check if 3000ms (3 seconds) have passed since entering this GREEN state.
      if (millis() - stateEnterTime >= 3000) {
        Serial.println("Timeout: GREEN -> BLUE");
        // If the timeout is reached, transition back to the BLUE state.
        currentState = STATE_BLUE;
        setLedColor(currentState);
        stateEnterTime = millis(); // Reset timer for the new state.
      }
      break;
  }
}

// This function sets the physical LED pins based on the current state number.
// The LED is common-anode, so LOW turns a color ON and HIGH turns if OFF.
void setLedColor(int state) {
  // Use a switch statement to apply the correct pin configuration for the desired color.
  switch (state) {
    
    // CASE: The desired state is DARK.
    case STATE_DARK:
      // Turn all colors OFF by setting their pins to HIGH.
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      break;

    // CASE: The desired state is RED.
    case STATE_RED:
      // Turn the RED color ON (LOW) and the others OFF (HIGH).
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      break;
    
    // CASE: The desired state is BLUE.
    case STATE_BLUE:
      // Turn the BLUE color ON (LOW) and the others OFF (HIGH).
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      break;
    
    // CASE: The desired state is GREEN.
    case STATE_GREEN:
      // Turn the GREEN color ON (LOW) and the others OFF (HIGH).
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      break;
  }
}
