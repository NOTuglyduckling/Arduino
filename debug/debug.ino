const int buttonPin = 2;  // Pin for On/Off button
const int ModePin = 3;    // Pin for Mode button

unsigned long lastButtonPress = 0; // Last button press timestamp
unsigned long debounceDelay = 50; // Debounce delay in milliseconds
int buttonState = HIGH;           // Current state of the On/Off button
int lastButtonState = HIGH;       // Last state of the On/Off button
int modeState = HIGH;             // Current state of the Mode button
int lastModeState = HIGH;         // Last state of the Mode button

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Configure On/Off button with pull-up
  pinMode(ModePin, INPUT_PULLUP);   // Configure Mode button with pull-up
  Serial.begin(9600);              // For debugging
}

void loop() {
  unsigned long currentMillis = millis(); // Get current time

  // On/Off Button
  int readingButton = digitalRead(buttonPin); // Read On/Off button state
  if (readingButton != lastButtonState) {    // If state has changed
    lastButtonPress = currentMillis;         // Reset debounce timer
  }
  if ((currentMillis - lastButtonPress) > debounceDelay) {
    if (readingButton == LOW && buttonState == HIGH) { // Detect press
      Serial.println("On/Off Button Pressed");
      buttonState = LOW; // Update button state
    }
    else if (readingButton == HIGH && buttonState == LOW) { // Detect release
      buttonState = HIGH; // Update button state
    }
  }
  lastButtonState = readingButton; // Update last state

  // Mode Button
  int readingMode = digitalRead(ModePin); // Read Mode button state
  if (readingMode != lastModeState) {     // If state has changed
    lastButtonPress = currentMillis;      // Reset debounce timer
  }
  if ((currentMillis - lastButtonPress) > debounceDelay) {
    if (readingMode == LOW && modeState == HIGH) { // Detect press
      Serial.println("Mode Button Pressed");
      modeState = LOW; // Update mode state
    }
    else if (readingMode == HIGH && modeState == LOW) { // Detect release
      modeState = HIGH; // Update mode state
    }
  }
  lastModeState = readingMode; // Update last state
}
