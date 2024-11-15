#include <LedControl.h>
#include <DHT11.h>

#define DATA_IN 12
#define CLK 11
#define LOAD 10
#define ModePin 2
#define BRIGHTNESS 0

LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);
DHT11 dht11(4);

int hours = 17, minutes = 9, seconds = 0; // edit here to set time for now
int h1, h2, m1, m2, t1, t2,hu1,hu2 = 0;
int temp = 0, humid = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
int Mode = 0, lastButtonState=LOW;  // Mode toggle for time/temperature/humidity display
unsigned long debounceTime = 200UL, lastDebounce = 0;

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
  {0x00, 0x02, 0x05, 0x02}, // °
  {0x0F, 0x08, 0x08, 0x0F}, // C
  {0x09, 0x04, 0x02, 0x09}, // %
  {0x09, 0x0F, 0x09, 0x09}, // H
};

void resetMatrix() {
  lc.clearDisplay(0);
}

void updateMatrix(int tl, int tr, int bl, int br) {
  resetMatrix();
  displayDigitRotated(tl, 0, 4);
  displayDigitRotated(tr, 4, 4);
  displayDigitRotated(bl, 0, 0);
  displayDigitRotated(br, 4, 0);
}

void displayDigitRotated(int digit, int rowOffset, int colOffset) {
  for (int col = 0; col < 4; col++) {
    byte colPattern = nombre[digit][col];
    for (int row = 0; row < 4; row++) {
      bool ledState = colPattern & (1 << (3 - row));
      lc.setLed(0, row + rowOffset, col + colOffset, ledState);
    }
  }
}

void setup() {
  lc.shutdown(0, false); // turn Matrix ON
  lc.setIntensity(0, BRIGHTNESS); // set brightness of LEDs
  lc.clearDisplay(0);
  pinMode(ModePin, INPUT);
}

void loop() {
  int reading = digitalRead(ModePin);
  // Button cycles through options with protection of debounce
  if ((reading == HIGH) && (lastButtonState == LOW) && (millis() - lastDebounce >= debounceTime)) {
    Mode = (Mode + 1) % 3;
    lastDebounce = millis();
  }
  lastButtonState=reading;
  
  unsigned long currentMillis = millis();
  // Matrix is updated every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // increment time variables accordingly
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
      }
    }

    // get temperature and humidity readings from DHT11 sensor
    dht11.readTemperatureHumidity(temp, humid);
   
    switch (Mode){
    case (0):
      h2 = hours % 10;
      h1 = hours / 10;
      m2 = minutes % 10;
      m1 = minutes / 10;
      updateMatrix(h1, h2, m1, m2);
      break;
    case (1):
      t2 = temp % 10;
      t1 = temp / 10;
      updateMatrix(t1, t2, 10, 11);
      break;
    case (2):
      hu2 = humid %10;
      hu1 = humid /10;
      updateMatrix(hu1,hu2,12,13);
      break;
    }
  }
}

