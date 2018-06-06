#include <Wire.h>
#include <Q2HX711.h>
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


// Q2HX711.DOUT  - pin
// Q2HX711.PD_SCK - pin
Q2HX711 scale(3, 4);


// initialize the library with the numbers of the interface pins

//int pinOutPiezo = 9;
int PUMP = 5;
//int pinOutLedFill = 10;
//int pinOutLedPanic = 13;
int BTN_CALIBRATE = 10; // Rood
int BTN_STOP = 13; // Wit
int BTN_FILL = 11; // Geel
int BTN_FLUSH = 12; // Blauw

const byte STATE_WAIT = 1;
const byte STATE_FILL = 2;
const byte STATE_CALIBRATE = 3;
const byte STATE_FLUSH = 4;

volatile byte state = STATE_WAIT;

// How many scale samples should we take for calibration?
int N_SCALE_SAMPLES = 20;

float TARE = 0.0;
float MAX_WEIGHT = 0.0;


/**
 * Show message on LCD screen. Clean up screen and move to 0,0.
 */
void screen(String msg) {

  lcd.clear();
  lcd.print(msg);
}


/**
 * Print on second line of screen.
 */
void screen2(String msg) {

  lcd.setCursor(0, 1);
  lcd.print(msg);
}


/**
 * Log to both screen and serial port.
 */
void log(String msg) {

  Serial.println(msg);
}


void log_and_screen(String msg) {

  log(msg);
  screen(msg);
}

void log_and_screen2(String msg) {

  log(msg);
  screen2(msg);
}


void setup() {

  // Set baud rate
  Serial.begin(38400);
  lcd.begin();
  lcd.backlight();

  log("Bottelmachine MK6.66");

  delay(2000);

  pinMode(PUMP, OUTPUT);
  // pinMode(pinOutLedFill, OUTPUT);
  // pinMode(pinOutLedPanic, OUTPUT);
  pinMode(BTN_CALIBRATE, INPUT);
  pinMode(BTN_STOP, INPUT);
  pinMode(BTN_FILL, INPUT);
  pinMode(BTN_FLUSH, INPUT);

  // interrupt #0 = digital pin 2 (on Uno)
  // interrupt #1 = digital pin 3 (on Uno)
  // attachInterrupt(0, panic, RISING);

  calibrate_tare();

  delay(1000);
}


void calibrate_tare() {

  log_and_screen("Calibrating tare...");

  TARE = 0.0;

  // Do 20 readings and take the average as the tare
  for (int i = 0; i < N_SCALE_SAMPLES; i++){
    TARE += scale.read();
  }

  TARE = TARE / float(N_SCALE_SAMPLES);

  log_and_screen2("Tare: " + String(TARE));  
}


/**
 * Determine the weight of one bottle, filled
 */
void calibrate() {

  log_and_screen("Calibrating fill weight...");

  MAX_WEIGHT = 0.0;

  // Do 20 readings and take the average as the fill weight
  for (int i = 0; i < N_SCALE_SAMPLES; i++){
    MAX_WEIGHT += scale.read();
  }

  MAX_WEIGHT = (MAX_WEIGHT / float(N_SCALE_SAMPLES)) - TARE;

  log_and_screen2("Fill weight: " + String(MAX_WEIGHT));
}


float get_weight() {

  return scale.read() - TARE;
}


boolean on(int btn) {

  return digitalRead(btn) == HIGH;
}


/**
 * Check buttons and set state accordingly.
 */
boolean set_state() {

  byte old_state = state;

  if (on(BTN_STOP)) {
    state = STATE_WAIT;
  } else if (on(BTN_CALIBRATE)) {
    state = STATE_CALIBRATE;
  } else if (on(BTN_FILL)) {
    state = STATE_FILL;
  } else if (on(BTN_FLUSH)) {
    state = STATE_FLUSH;
  }

  return state != old_state;
}


void startPump() {
  analogWrite(PUMP, 255);
}


/**
 * Set pump to half speed.
 */
void dimPump() {
  analogWrite(PUMP, 128);
}


void stopPump() {
  analogWrite(PUMP, 0);
}


void loop() {

  boolean changed = set_state();

  if (state == STATE_FILL && changed) {
    startPump();
    log("Filling");    
  } else if (state == STATE_WAIT && changed) {
    stopPump();
    log("Stopped");    
  } else if (state == STATE_CALIBRATE) {
    calibrate();
    state = STATE_WAIT;
  } else if (state == STATE_FLUSH && changed) {
    log("Flushing");
    startPump();
  }

  if (state == STATE_FILL) {

    float weight = get_weight();

    // Print % filled on second line
    screen2(String(int(weight / MAX_WEIGHT) * 100) + "%");

    if (weight >= MAX_WEIGHT) {
      screen2("Full");
      stopPump();
      state = STATE_WAIT;
    } else if (weight > (MAX_WEIGHT * 0.8)) {
      dimPump();
    }
  }

  delay(500);
}
