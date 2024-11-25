#include <LedControl.h>
#include <DHT11.h>
#include <DS3231.h>
#include <stdlib.h>

DS3231 myRTC;
bool h12Flag;
bool pmFlag;
#define DATA_IN 12
#define CLK 11
#define LOAD 10
#define ModePin 2
#define OnOff 3
#define BRIGHTNESS 0
#define SPEED 1
#define RAND_MAX 8

LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);
DHT11 dht11(4);

int hours = 16, minutes = 19, seconds = 0; // edit here to set time for now
int h1, h2, m1, m2, t1, t2,hu1,hu2 = 0;
int temp = 0, humid = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
int Mode = 0, lastButtonState=LOW, lastOnState=LOW;  // Mode toggle for time/temperature/humidity display and On/Off
unsigned long debounceTime = 200, lastDebounce = 0, lastBounce = 0;
bool ScreenState=false;

byte nombre[][4] = {
  {0x0F, 0x09, 0x09, 0x0F}, // 0
  {0x02, 0x02, 0x06, 0x02}, // 1
  {0x08, 0x0F, 0x01, 0x0F}, // 2
  {0x01, 0x07, 0x01, 0x0F}, // 3
  {0x01, 0x0F, 0x09, 0x09}, // 4
  {0x01, 0x0E, 0x08, 0x0F}, // 5
  {0x09, 0x0E, 0x08, 0x07}, // 6
  {0x08, 0x04, 0x02, 0x0F}, // 7
  {0x09, 0x06, 0x09, 0x06}, // 8
  {0x01, 0x07, 0x09, 0x06}, // 9
  {0x00, 0x02, 0x05, 0x02}, // °
  {0x0F, 0x08, 0x08, 0x0F}, // C
  {0x09, 0x04, 0x02, 0x09}, // %
  {0x09, 0x0F, 0x09, 0x09}, // H
};

void setup() {
  lc.shutdown(0, ScreenState); // turn Matrix ON
  lc.setIntensity(0, BRIGHTNESS); // set brightness of LEDs
  lc.clearDisplay(0);
  pinMode(ModePin, INPUT);
  pinMode(OnOff, INPUT);
  srand(time(NULL));   // Initialization, should only be called once.
  Wire.begin();
  myRTC.setClockMode(false);  // set to 24h 
  myRTC.setYear(2024);
  myRTC.setMonth(11);
  myRTC.setDate(25);
  myRTC.setDoW(1);
  myRTC.setHour(17);
  myRTC.setMinute(2);
  myRTC.setSecond(50);
}

//############################################################################ END OF SETUP ##################################################################################


void updateMatrix(int tl, int tr, int bl, int br) {
  resetMatrix();
  displayDigitRotated(tl, 0, 4);
  displayDigitRotated(tr, 4, 4);
  displayDigitRotated(bl, 0, 0);
  displayDigitRotated(br, 4, 0);
}

void resetMatrix() {
  lc.clearDisplay(0);
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

//################################################################################ MAIN ###################################################################################

void loop() {
  int reading = digitalRead(ModePin);
  // Button cycles through options with protection of debounce
  if ((reading == HIGH) && (lastButtonState == LOW) && (millis() - lastDebounce >= debounceTime)) {
    Mode = (Mode + 1) % 4;
    lastDebounce = millis();
  }
  lastButtonState=reading;

  int read = digitalRead(OnOff);
  // Button for turing matrix On/Off
  if ((read == HIGH) && (lastOnState == LOW) && (millis() - lastBounce >= debounceTime)) {
    ScreenState=!ScreenState;
    lc.shutdown(0, ScreenState);
    lastBounce = millis();
  }
  lastOnState=read;

  unsigned long currentMillis = millis();
  // Matrix is updated every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis +=1000;
    // increment time variables accordingly
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        // Every hour update Timekeeping
        seconds = myRTC.getSecond();
        minutes = myRTC.getMinute();
        hours = myRTC.getHour(h12Flag, pmFlag);
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

