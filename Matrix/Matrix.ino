#include <LedControl.h>

#define DATA_IN 12
#define CLK 11
#define LOAD 10
LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);

#define MESH A0   // Button for hour adjustment
#define MESM A1   // Button for minute adjustment

// Binary patterns for digits 0-9, arranged in a 4x4 format
byte nombre[][4] = {
  {0xF, 0x9, 0x9, 0xF},  // 0 
  {0x4, 0xC, 0x4, 0x4},  // 1
  {0xF, 0x1, 0xF, 0x8},  // 2
  {0xF, 0x1, 0xF, 0x1},  // 3
  {0x9, 0x9, 0xF, 0x1},  // 4
  {0xF, 0x8, 0xF, 0x1},  // 5
  {0xF, 0x8, 0xF, 0x9},  // 6
  {0xF, 0x1, 0x2, 0x4},  // 7
  {0xF, 0x9, 0xF, 0x9},  // 8
  {0xF, 0x9, 0xF, 0x1}   // 9
};


int hores = 0, minuts = 0;
int h1, h2, m1, m2 = 0;

// Reset the entire matrix display
void resetMatrix() {
  lc.clearDisplay(0); // Clear all LEDs on the display
}

// Update time variables and wrap around when limits are reached
void timeAdapt() {
  if (minuts >= 60) {
    minuts = 0;
    hores++;
    if (hores >= 24) hores = 0;
  }
}

// Function to display a rotated 4x4 digit at a specified row offset
void displayDigitRotated(int digit, int rowOffset) {
  for (int col = 0; col < 4; col++) {
    byte colPattern = nombre[digit][col];  // Get the original column pattern for the digit
    for (int row = 0; row < 4; row++) {
      bool ledState = colPattern & (1 << (3 - row)); // Extract bit for LED state
      lc.setLed(0, row + rowOffset, 7 - col, ledState);  // Rotated position: row and column swapped
    }
  }
}

void setup() {
  lc.shutdown(0, false);       // Wake up MAX7219
  lc.setIntensity(0, 8);       // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);          // Clear the display
  pinMode(MESH, INPUT);
  pinMode(MESM, INPUT);
}

void loop() {
  if (digitalRead(MESH) == HIGH) {
    hores++;
    timeAdapt();
    delay(100);
  }

  if (digitalRead(MESM) == HIGH) {
    minuts++;
    timeAdapt();
    delay(100);
  }

  // Calculate the hour and minute digits
  h2 = hores % 10;
  h1 = hores / 10;
  m2 = minuts % 10;
  m1 = minuts / 10;

  resetMatrix();

  // Display each digit with a 90-degree rotation at appropriate row offsets
  displayDigitRotated(h1, 0);  // Display the tens place of the hour at row offset 0
  displayDigitRotated(h2, 4);  // Display the units place of the hour at row offset 4
  displayDigitRotated(m1, 0);  // Display the tens place of the minute at row offset 0 (or 8 for a new display)
  displayDigitRotated(m2, 4);  // Display the units place of the minute at row offset 4 (or 12 for a new display)

  delay(500); // Update display every 500 ms
}
