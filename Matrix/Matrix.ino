#include <LedControl.h>

#define DATA_IN 12
#define CLK 11
#define LOAD 10
LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);

#define MESH A0   // Button for hour adjustment
#define MESM A1   // Button for minute adjustment

int hours = 0, minutes = 0;
int h1, h2, m1, m2 = 0;

byte nombre[][4] = {
  {0x0F, 0x09, 0x09, 0x0F}, // 0
  {0x02, 0x02, 0x06, 0x02}, // 1
  {0x07, 0x02, 0x01, 0x06}, // 2
  {0x0F, 0x01, 0x03, 0x0F}, // 3
  {0x01, 0x01, 0x0F, 0x09}, // 4
  {0x08, 0x0F, 0x01, 0x0F}, // 5
  {0x09, 0x0E, 0x08, 0x07}, // 6
  {0x08, 0x04, 0x02, 0x0F}, // 7
  {0x09, 0x06, 0x09, 0x06}, // 8
  {0x01, 0x07, 0x09, 0x06}  // 9
};

// Reset the entire matrix display
void resetMatrix() {
  lc.clearDisplay(0); // Clear all LEDs on the display
}

// Update time variables and wrap around when limits are reached
void timeAdapt() {
  if (minutes >= 60) {
    minutes = 0;
    hours++;
    if (hours >= 24) hours = 0;
  }
}

// Function to display a 4x4 digit rotated 90 degrees to the right
void displayDigitRotated(int digit, int rowOffset, int colOffset) {
  for (int col = 0; col < 4; col++) {
    byte colPattern = nombre[digit][col];  // Get the original column pattern for the digit
    for (int row = 0; row < 4; row++) {
      bool ledState = colPattern & (1 << (3 - row)); // Extract bit for LED state
      lc.setLed(0, row + rowOffset, col + colOffset, ledState);  // Set LED based on row and column
    }
  }
}

void setup() {
  lc.shutdown(0, false);       // Wake up MAX7219
  lc.setIntensity(0, 8);        // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);           // Clear the display
  pinMode(MESH, INPUT);
  pinMode(MESM, INPUT);
}

void loop() {
  if (digitalRead(MESH) == HIGH) {
    hours++;
    timeAdapt();
    delay(100);  // debounce delay
  }

  if (digitalRead(MESM) == HIGH) {
    minutes++;
    timeAdapt();
    delay(100);  // debounce delay
  }

  // Calculate the hour and minute digits
  h2 = hours % 10;      // Units place of hours
  h1 = hours / 10;      // Tens place of hours
  m2 = minutes % 10;    // Units place of minutes
  m1 = minutes / 10;    // Tens place of minutes

  // Debugging: Ensure correct values for h1, h2, m1, m2
  Serial.print("Hours: ");
  Serial.print(h1);
  Serial.print(h2);
  Serial.print(" Minutes: ");
  Serial.print(m1);
  Serial.println(m2);

  resetMatrix();

  // Display hours (top two quadrants)
  displayDigitRotated(h1, 0, 4);  // Tens place of hours in top-left
  displayDigitRotated(h2, 4, 4);  // Units place of hours in top-right

  // Display minutes (bottom two quadrants)
  displayDigitRotated(m1, 0, 0);  // Tens place of minutes in bottom-left
  displayDigitRotated(m2, 4, 0);  // Units place of minutes in bottom-right

  delay(500); // Update display every 500 ms
}
