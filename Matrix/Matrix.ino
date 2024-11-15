#include <LedControl.h>	
#include <DHT11.h>

#define DATA_IN 12
#define CLK 11
#define LOAD 10

LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);
DHT11 dht11(4); 

const int buttonPin = 2;
const int ModePin = 3;
int hours = 20, minutes = 29, seconds =0;
int h1, h2, m1, m2, t1, t2 = 0;

unsigned long previousMillis = 0;  // Stores the last time the time was updated
const long interval = 1000;        // Interval in milliseconds (1000 ms = 1 second)
int state = true, Mode = true;
int reading, readingMode;
int previous = LOW;
int previousMode = LOW;
int temp =0, humid=0 ;

unsigned long time = 0;
unsigned long debounce = 200UL;

byte nombre[][4] = {
  {0x0F, 0x09, 0x09, 0x0F}, // 0
  {0x02, 0x02, 0x06, 0x02}, // 1
  {0x0F, 0x02, 0x09, 0x06}, // 2
  {0x0F, 0x01, 0x03, 0x0F}, // 3
  {0x01, 0x01, 0x0F, 0x09}, // 4
  {0x01, 0x0F, 0x08, 0x0F}, // 5
  {0x09, 0x0E, 0x08, 0x07}, // 6
  {0x08, 0x04, 0x02, 0x0F}, // 7
  {0x09, 0x06, 0x09, 0x06}, // 8
  {0x01, 0x07, 0x09, 0x06}, // 9
  {0x0F, 0x08, 0x08, 0x0F}, // C
  {0x00, 0x02, 0x05, 0x02}  // °
};

// Reset the entire matrix display
void resetMatrix() {
  lc.clearDisplay(0); // Clear all LEDs on the display
}

void updateMatrix(int tl,int tr,int bl,int br){
  resetMatrix();

    // Display hours (top two quadrants)
    displayDigitRotated(h1, 0, 4);  // Tens place of hours in top-left
    displayDigitRotated(h2, 4, 4);  // Ones place of hours in top-right

    // Display minutes (bottom two quadrants)
    displayDigitRotated(m1, 0, 0);  // Tens place of minutes in bottom-left
    displayDigitRotated(m2, 4, 0);  // Ones place of minutes in bottom-right
}

// Update time variables and wrap around when limits are reached
void timeAdapt() {
  
  minutes++;
  seconds=0;
  
  if (minutes >= 60) {
    minutes = 0;    // Reset minutes to 0
    hours++;        // Increment hours
    if (hours >= 24) {
      hours = 0;    // Reset hours to 0 after 23
    }
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
  lc.setIntensity(0, 0);       // Set brightness level (0 is min, 15 is max)
  lc.clearDisplay(0);          // Clear the display
  pinMode(buttonPin, INPUT);
  pinMode(ModePin,INPUT);
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time in milliseconds
  // Check if 1 second (1000 ms) has passed
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the time was updated
    previousMillis = currentMillis;
    seconds++;
    // Automatically increment minutes
    if (seconds>=60){
      timeAdapt();  // Adjust time if necessary (e.g., overflow)
      h2 = hours % 10;
      h1 = hours / 10;
      m2 = minutes % 10;
      m1 = minutes / 10;
      int result = dht11.readTemperatureHumidity(temp, humid);
      t2 = temp %10;
      t1 = temp/10;
      readingMode = digitalRead(ModePin);

      if (readingMode == HIGH && previousMode == LOW && millis() - time > debounce){
        if (Mode == false)
        Mode = true;
        else
        Mode = false;

        time = millis();
      }
      if (Mode){
        updateMatrix(h1,h2,m1,m2);
      } else {
        updateMatrix(t1,t2,11,10);
      }
  
      previousMode = readingMode;
      
    }
    
  }
  reading = digitalRead(buttonPin);

  if (reading == HIGH && previous == LOW && millis() - time > debounce)
  {
    if (state == false)
      state = true;
    else
      state = false;

    time = millis();
  }

  lc.shutdown(0, state);

  previous = reading;
}
