#include <LedControl.h>

// Define the pins for the MAX7219 control
#define DATA_IN 12
#define CLK 11
#define LOAD 10
LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1); // Initialize with one device

// Define pins for MESH and MESM (hour and minute increment buttons)
#define MESH A0   // Adjust this pin as needed
#define MESM A1   // Adjust this pin as needed

// Binary patterns for digits 0-9 for a 4x4 display within the 8x8 grid
byte nombre[][4] = {
  {0x0F, 0x09, 0x09, 0x0F}, // 0
  {0x01, 0x03, 0x01, 0x01}, // 1
  {0x0F, 0x01, 0x0F, 0x08}, // 2
  {0x0F, 0x01, 0x0F, 0x01}, // 3
  {0x09, 0x09, 0x0F, 0x01}, // 4
  {0x0F, 0x08, 0x0F, 0x01}, // 5
  {0x0F, 0x08, 0x0F, 0x09}, // 6
  {0x0F, 0x01, 0x02, 0x04}, // 7
  {0x0F, 0x09, 0x0F, 0x09}, // 8
  {0x0F, 0x09, 0x0F, 0x01}  // 9
};

int hores = 0, minuts = 0;
int h1, h2, m1, m2 = 0;

// Function to display a 4x4 digit at a specified column offset
void displayDigit(int digit, int colOffset) {
  for (int row = 0; row < 4; row++) {
    byte rowPattern = nombre[digit][row];  // Get row pattern for the digit
    for (int col = 0; col < 4; col++) {
      bool ledState = rowPattern & (1 << (3 - col)); // Check each bit for LED state
      lc.setLed(0, row, col + colOffset, ledState);  // Set LED on or off
    }
  }
}

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

  // Display each digit at appropriate column offsets
  displayDigit(h1, 0);  // Display the tens place of the hour at column offset 0
  displayDigit(h2, 4);  // Display the units place of the hour at column offset 4
  displayDigit(m1, 0);  // Display the tens place of the minute at column offset 0 (or 8 for a new display)
  displayDigit(m2, 4);  // Display the units place of the minute at column offset 4 (or 12 for a new display)

  delay(500); // Update display every 500 ms
}
