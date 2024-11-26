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
#define TEMP_SYMBOL 10
#define CELSIUS_SYMBOL 11
#define HUMIDITY_SYMBOL 12
#define PERCENT_SYMBOL 13
#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define JoyButton 7  // Arduino pin connected to SW  pin

#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

LedControl lc = LedControl(DATA_IN, CLK, LOAD, 1);
DHT11 dht11(4);


// SNake
unsigned long lastMoveTime = 0;  // Stores the last update time
const long snakeSpeed = 200;    // Delay in milliseconds between moves
int command = COMMAND_NO;
int xValue = 0;
int yValue = 0;
int row=0;int col=0;

// Time and humidity
int hours,minutes,seconds;
int h1, h2, m1, m2, t1, t2,hu1,hu2 = 0;
int temp = 0, humid = 0;

// Buttons
unsigned long previousMillis = 0;
const long interval = 1000;
int Mode = 3; //Mode at startup
int lastButtonState=LOW, lastOnState=LOW;  // Mode toggle for time/temperature/humidity display and On/Off
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
  Wire.begin();
  myRTC.setClockMode(false);  // set to 24h 
  myRTC.setYear(2024);
  myRTC.setMonth(11);
  myRTC.setDate(25);
  myRTC.setDoW(1);
  myRTC.setHour(21);// edit here to set time for now
  myRTC.setMinute(3);
  myRTC.setSecond(10);
  seconds = myRTC.getSecond();
  minutes = myRTC.getMinute();
  hours = myRTC.getHour(h12Flag, pmFlag);
}

//############################################################################ END OF SETUP ##################################################################################


void updateMatrix(int tl, int tr, int bl, int br) {
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

void displayTime() {
    h2 = hours % 10;
    h1 = hours / 10;
    m2 = minutes % 10;
    m1 = minutes / 10;
    updateMatrix(h1, h2, m1, m2);
}

void displayTemperature() {
    t2 = temp % 10;
    t1 = temp / 10;
    updateMatrix(t1, t2, TEMP_SYMBOL, CELSIUS_SYMBOL);
}

void displayHumidity() {
    hu2 = humid % 10;
    hu1 = humid / 10;
    updateMatrix(hu1, hu2, HUMIDITY_SYMBOL, PERCENT_SYMBOL);
}

bool isButtonPressed(int pin, int &lastState, unsigned long &lastBounceTime, unsigned long debounceDelay) {
    int currentState = digitalRead(pin);
    if ((currentState == HIGH) && (lastState == LOW) && (millis() - lastBounceTime >= debounceDelay)) {
        lastBounceTime = millis();
        lastState = currentState;  
        return true;           
    }
    lastState = currentState;
    return false;
}

void ModeChangeAnimation() {
    for (int diag = 0; diag < 8; diag++) {
        for (int row = 0; row <= diag; row++) {
            int col = diag - row;
            if (col < 8 && row < 8) lc.setLed(0, row, col, true);
        }
        delay(50);
    }
    delay(100);
    resetMatrix();
}

void snake(){

// Get the current time
  unsigned long currentMillis = millis();

  // Only update movement if enough time has passed
  if (currentMillis - lastMoveTime >= snakeSpeed) {
    lastMoveTime = currentMillis;  // Update the last move time
    xValue = analogRead(VRX_PIN);
    yValue = analogRead(VRY_PIN);
    command = COMMAND_NO;

    // Determine direction
    if (xValue < LEFT_THRESHOLD) command |= COMMAND_UP;
    else if (xValue > RIGHT_THRESHOLD) command |= COMMAND_DOWN;
    if (yValue < UP_THRESHOLD) command |= COMMAND_RIGHT;
    else if (yValue > DOWN_THRESHOLD) command |= COMMAND_LEFT;

    // Clear previous position
    lc.setLed(0, row, col, false);


    if ((command & COMMAND_LEFT) && col>0) col--;
    if ((command & COMMAND_RIGHT) && col<7) col++;
    if ((command & COMMAND_UP) && row<7) row++;
    if ((command & COMMAND_DOWN) && row>0) row--;
    
    lc.setLed(0,row,col,true);
  }
}

//################################################################################ MAIN ###################################################################################

void loop() {
  if (isButtonPressed(ModePin, lastButtonState, lastDebounce, debounceTime)) {
      Mode = (Mode + 1) % 4;  // Cycle through modes
      ModeChangeAnimation();
  }

  if (isButtonPressed(OnOff, lastOnState, lastBounce, debounceTime)) {
      ScreenState = !ScreenState;    // Toggle screen state
      lc.shutdown(0, ScreenState);  // Update the LED matrix
  }
  if (Mode==3){
    snake();
    return;
  }

  seconds = myRTC.getSecond();
  if (seconds==0){
    minutes = myRTC.getMinute();
    hours = myRTC.getHour(h12Flag, pmFlag);
  }

  // get temperature and humidity readings from DHT11 sensor
  dht11.readTemperatureHumidity(temp, humid);

  if (!ScreenState){
    switch (Mode) {
    case 0: displayTime(); break;
    case 1: displayTemperature(); break;
    case 2: displayHumidity(); break;
    case 3: snake(); break;
    }
  } else {
    resetMatrix();
  }
}

