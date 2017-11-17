#include <Wire.h> 
#include <Q2HX711.h>
#include <LiquidCrystal_I2C.h>

// Juiste LCD library installeren:
// ga naar <sketch directory>/libraries
// maak dir LiquidCrystal_I2C aan
// cd LiquidCrystal_I2C
// git clone https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
// cd Arduino-LiquidCrystal-I2C-library
// mv * ..
// cd ..
// rm -rf Arduino-LiquidCrystal-I2C-library
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Q2HX711.DOUT  - pin 
// Q2HX711.PD_SCK - pin 
Q2HX711 scale(3, 4);
// initialize the library with the numbers of the interface pins


//int pinOutPiezo = 9;
int pinOutPump = 5;
//int pinOutLedFill = 10;
//int pinOutLedPanic = 13;
int pinInCalibrate = 10;
int pinInPanic = 13; // witte knopje
int pinInStart = 11; 
int pinInExtra = 12; 

const byte STATE_WAIT = 1;
const byte STATE_FILL = 2;
const byte STATE_PANIC = 3;

volatile byte state = STATE_WAIT;
volatile int counter = 0;
int buttonstate = LOW;
float tarra = 0;

void fles_vol() {
  /*Tone needs 2 arguments, but can take three
    1) Pin#
    2) Frequency (Hz) - this is in hertz (cycles per second) which determines the pitch of the noise made
    3) Duration (ms) - how long teh tone plays
  */
  analogWrite(pinOutPump, 0);
//  tone(pinOutPiezo, 1000, 400);
//  delay(600);
//  tone(pinOutPiezo, 1040, 400);
//  delay(600);
//  tone(pinOutPiezo, 1080, 400);

  state = STATE_WAIT;
}

void panic(){
  // catch the interrupt. Let the loop handle it
  state = STATE_PANIC;
}
  
void setup() {

  Serial.begin(38400);
  Serial.println("LCD Demo");

  lcd.begin();                      // initialize the lcd 
  Serial.println("init");
  lcd.backlight();
  Serial.println("backlight");
//
//  lcd.clear();
//  Serial.println("clear");
//  lcd.setCursor(0,0);
//  lcd.print("Hello, world!");
//  Serial.println("Hello there");


  
  // put your setup code here, to run once:
//  pinMode(pinOutPiezo, OUTPUT);
  pinMode(pinOutPump, OUTPUT);
//  pinMode(pinOutLedFill, OUTPUT);
//  pinMode(pinOutLedPanic, OUTPUT);
  
  pinMode(pinInCalibrate, INPUT);
  pinMode(pinInPanic, INPUT);
  pinMode(pinInStart, INPUT);
  pinMode(pinInExtra, INPUT);
  
  // interrupt #0 = digital pin 2 (on Uno)
  // interrupt #1 = digital pin 3 (on Uno)
  // attachInterrupt(0, panic, RISING);
  
//  fles_vol();

  Serial.println("HX711 Demo");

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");

  tarra = 0.0;
  for (int i=0; i<20; i++){
    tarra += scale.read();      // print a raw reading from the ADC
  }
  tarra = tarra / 20.0;
  Serial.print("Tarra = ");
  Serial.println(tarra);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(250);
  counter += 1;

  analogWrite(pinOutPump, 0);

  buttonstate = digitalRead(pinInPanic);

  if (buttonstate == HIGH){
    Serial.println("ik ben high"); 
  } else {
    Serial.println("ik ben looow");     
  }

//   buttonstate = digitalRead(pinInCalibrate);


  
//  if (state == STATE_PANIC){
//    state = STATE_WAIT;
//    Serial.println("PANIC");
//    analogWrite(pinOutPump, 0);
//    digitalWrite(pinOutLedPanic, HIGH);
//    // tone(pinOutPiezo, 1000, 300);
//    delay(400);
//    digitalWrite(pinOutLedPanic, LOW);
//    //tone(pinOutPiezo, 900, 200);
//    delay(300);
//    digitalWrite(pinOutLedPanic, HIGH);
//    //tone(pinOutPiezo, 900, 200);
//    delay(300);
//    digitalWrite(pinOutLedPanic, LOW);
//    delay(300);
//  }

  Serial.print("one reading:\t");
  Serial.println(scale.read() - tarra);

  if ( (scale.read() - tarra) > 100000) {
    Serial.print("zwaar!! : ");
    int mapped = map(scale.read() - tarra, -10000, 500000, 0, 900);
    Serial.println(mapped);
    analogWrite(pinOutPump, mapped);
    delay(2000);
  }
  
  delay(500);
}
