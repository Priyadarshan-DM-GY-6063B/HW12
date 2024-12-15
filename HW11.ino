// Define LED pins
const int ledPins[6] = {2, 3, 4, 5, 6, 7}; // Pins for binary seconds
const int minuteLed = 8;                  // Pin for minute LED

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600); // Set baud rate to 9600
  
  // Set all LED pins as OUTPUT
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(minuteLed, OUTPUT);

  // Print startup message
  Serial.println("Starting Binary LED Clock...");
}

void loop() {
  // Calculate elapsed time
  unsigned long milliseconds = millis();   // Total milliseconds since the program started
  int seconds = (milliseconds / 1000) % 60; // Elapsed seconds (0–59)
  int minutes = (milliseconds / 60000);     // Elapsed minutes

  // Display seconds in binary on the 6 LEDs
  Serial.print("Seconds: ");
  Serial.print(seconds);
  Serial.print(" | Binary: ");
  
  for (int i = 5; i >= 0; i--) { // Print binary representation (MSB to LSB)
    int bitValue = (seconds >> i) & 1;
    Serial.print(bitValue); // Display the bit value (0 or 1)
    digitalWrite(ledPins[i], bitValue); // Set the LED ON or OFF based on the bit value
  }
  
  Serial.print(" | Minutes: ");
  Serial.println(minutes);

  // Light up the minute LED
  if (minutes % 2 == 0) { // Toggle LED every minute
    digitalWrite(minuteLed, HIGH);
  } else {
    digitalWrite(minuteLed, LOW);
  }

  delay(10); // Short delay to ensure smooth operation
}
