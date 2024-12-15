// Pin Definitions
const int button1Pin = 11;    // Button 1 connected to Pin 11
const int button2Pin = 12;    // Button 2 connected to Pin 12
const int potPin = A0;        // Potentiometer connected to A0
const int greenLEDPin = 9;    // Green LED for success
const int redLEDPin = 10;     // Red LED for idle/failure

// Variables
bool potSet = false;           // Tracks if potentiometer is set
bool button1Pressed = false;   // Tracks if Button 1 is pressed
bool button2Pressed = false;   // Tracks if Button 2 is pressed
int potValue = 0;              // Current potentiometer value

// Potentiometer Thresholds
const int potMiddleMin = 450;
const int potMiddleMax = 570;

// Timing Variables
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long stepTimeout = 0;
const unsigned long debounceDelay = 50; // 50ms debounce delay
const unsigned long inputTimeout = 10000; // 10 seconds timeout for user input

void setup() {
  // Initialize pins
  pinMode(button1Pin, INPUT_PULLUP); // Button 1 with pull-up
  pinMode(button2Pin, INPUT_PULLUP); // Button 2 with pull-up
  pinMode(greenLEDPin, OUTPUT);      // Green LED
  pinMode(redLEDPin, OUTPUT);        // Red LED

  // Initial states for LEDs
  digitalWrite(greenLEDPin, LOW);    // Green LED OFF
  digitalWrite(redLEDPin, HIGH);     // Red LED ON (idle)

  // Serial monitor initialization
  Serial.begin(9600);
  Serial.println("Password Machine Initialized");
}

void loop() {
  if (!potSet) {
    checkPotentiometer();
  } else if (!button1Pressed) {
    if (digitalRead(button2Pin) == HIGH) {  // If Button 2 pressed before Button 1
      signalFailure("Incorrect sequence before pressing Button 1. Try again...");
      digitalWrite(redLEDPin, HIGH);
    } else if (validatePotentiometer()) {  // Validate potentiometer again
      checkButton(button1Pin, button1Pressed, lastDebounceTime1, "Button 1 Pressed. Now press Button 2.");
    }
  } else if (!button2Pressed) {
    checkButton(button2Pin, button2Pressed, lastDebounceTime2, "Button 2 Pressed. Validating...");
  } else {
    signalSuccess();
  }

  // Timeout check
  if (stepTimeout > 0 && millis() - stepTimeout > inputTimeout) {
    signalFailure("Timeout: User took too long. Resetting...");
  }
}

// Function to check the potentiometer
void checkPotentiometer() {
  potValue = analogRead(potPin);
  Serial.print("Potentiometer Value: ");
  Serial.println(potValue);

  if (validatePotentiometer()) {
    Serial.println("Potentiometer set in Middle Position. Now press Button 1.");
    potSet = true;
    stepTimeout = millis();         // Start timeout timer
    digitalWrite(redLEDPin, LOW);   // Turn off Red LED
    delay(1000);                    // Allow user time to prepare
  }
}

// Function to validate potentiometer position
bool validatePotentiometer() {
  potValue = analogRead(potPin); // Re-read the potentiometer value
  if (potValue < potMiddleMin || potValue > potMiddleMax) {
    Serial.println("Potentiometer out of range. Adjust it to the middle position.");
    digitalWrite(redLEDPin, HIGH);
    return false;
  }
  return true;
}

// Function to check a button press
void checkButton(int buttonPin, bool &buttonState, unsigned long &lastDebounceTime, const char *message) {
  static int lastState = HIGH; // Default state is HIGH (unpressed)
  int reading = digitalRead(buttonPin);

  // If the button state has changed
  if (reading != lastState) {
    lastDebounceTime = millis(); // Reset debounce timer
  }

  // Only register the press if the state is stable for debounceDelay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == HIGH) { // Button pressed
      Serial.println(message);
      buttonState = true;
      stepTimeout = millis(); // Reset timeout timer for next step
      delay(500);             // Stabilization delay
    }
  }

  lastState = reading; // Save the current state
}

// Function to indicate success
void signalSuccess() {
  digitalWrite(greenLEDPin, HIGH); // Turn on Green LED
  Serial.println("Success! Green LED Glowing.");
  delay(2000); // Keep LED on for 2 seconds
  resetMachine();
}

// Function to indicate failure
void signalFailure(const char *errorMessage) {
  digitalWrite(redLEDPin, HIGH); // Turn on Red LED
  digitalWrite(greenLEDPin, LOW); // Turn off Green LED
  Serial.println(errorMessage);
  delay(2000); // Keep Red LED on for 2 seconds
  resetMachine();
}

// Function to reset the machine
void resetMachine() {
  potSet = false;
  button1Pressed = false;
  button2Pressed = false;
  stepTimeout = 0;
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, HIGH); // Turn on Red LED
  Serial.println("Machine Reset. Set the potentiometer again.");
}