/*
  This script moves an actuator until maximum analog input is
  acheived.  While it's example application is to track a light source
  with a solar cell that is attached to the end of the actuator, it
  could be used to track maximum input of anything attached to the
  LIGHT_SENSOR_PIN.  maybe like sound source tracking too?  :)

*/

#include <Arduino.h>

#define ACTUATOR_AMP_SENSOR_PIN A5
#define ACTUATOR_PWM_PIN 11
#define ACTUATOR_DIR_PIN 12
#define LIGHT_SENSOR_PIN A4

// MAX speed is 255 - this is the value that gets written
// to the ACTUATOR_PWM_PIN.  Note do not change or limit
// detection will no work
#define ACTUATOR_SPEED  255

// when the amp sensor analog value is between these, it is
// considered to be at rest (no current being drawn).
#define ZEROAMP_RANGE_LOW 505
#define ZEROAMP_RANGE_HIGH 520

// directions for the actuator (value gets set to ACTUATOR_DIR_PIN)
#define RETRACT 0
#define EXTEND 1

// don't move if sampled light input value is withing
//STILL_TOLERANCE of g_lastInputValue
#define STILL_TOLERANCE 2
// move actuator until drop from max is less than
#define PETAL_SEEKING_TOLERANCE 2

// minimum number of milliseconds delay between sampling the ACS712
// recommended in manufacturer sample source
#define SAMPLING_DELAY 1

int g_currentDirection = EXTEND;
int g_lastInputValue = 0;


// limited to 255 char strings
void serialPrintf(char const *format, ...) {
  char buffer[255];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  Serial.println(buffer);
}


boolean isActuatorMoving() {
  int val = analogRead(ACTUATOR_AMP_SENSOR_PIN);
  // serialPrintf("Amp sensor reading: %d", val);
  return val < ZEROAMP_RANGE_LOW || val > ZEROAMP_RANGE_HIGH;
}

void stopActuator() {
  analogWrite(ACTUATOR_PWM_PIN, 0);
}

boolean almostEqual(int a, int b, int tolerance) {
  return a >= b - tolerance && a <= b + tolerance;
}

// mSeconds = optionally move in direction for n milliseconds and then stop
// returns false if mSeconds != 0 and at end of travel
boolean moveActuator(int direction, int mSeconds=0) {
  // serialPrintf("moving actuator direction=%d", direction);
  analogWrite(ACTUATOR_PWM_PIN, ACTUATOR_SPEED);
  digitalWrite(ACTUATOR_DIR_PIN, direction);
  if(mSeconds > 0){
    delay(mSeconds);
    boolean stillMoving = isActuatorMoving();
    stopActuator();
    return stillMoving;
  }
  return true;
}

// returns the number of milliseconds taken
unsigned long moveActuatorToEnd(int direction) {
  long startMs = millis();
  moveActuator(direction);
  while (isActuatorMoving()){
    delay(SAMPLING_DELAY);  // slight delay between sampling the ACS712 module is recommended
  }
  stopActuator();
  long endMs = millis();
  return endMs - startMs;
}

void changeDirection(int direction = -1) {
  if (direction == -1)
    direction = g_currentDirection + 1;
  // mask to single bit - 0 or 1, so 0 + 1 becomes 1 and 1 + 1 becomes 0
  g_currentDirection = direction & 1;
}


// finds the max input by moving through the full range of motion
void calibrate() {
  moveActuatorToEnd(EXTEND);   // start fully extended

  long tMaxInput = 0;  // time elapsed while retracting that max input was achieved
  int maxInput = 0;   // what was the input

  long startMs = millis();
  moveActuator(RETRACT);

  while (isActuatorMoving()) {
    int inputValue = analogRead(LIGHT_SENSOR_PIN);
    if (inputValue > maxInput) {
      maxInput = inputValue;
      tMaxInput = millis() - startMs;
      delay(SAMPLING_DELAY);
    }
  }
  long endMs = millis();

  // move back to point in time when at max
  moveActuator(EXTEND, endMs - startMs - tMaxInput);
}

// returns false if at end of travel
boolean seekHighInput(int pin, int direction, int inputValue) {
  int nextInputValue = inputValue;
  int maxInputValue = inputValue;
  boolean stillMoving;
  boolean atMax;

  serialPrintf("seeking to high input in direction %d. inputValue=%d",
    direction, inputValue);

  moveActuator(direction);

  do {
    delay(SAMPLING_DELAY);
    stillMoving = isActuatorMoving();
    if (nextInputValue > maxInputValue)
      maxInputValue = nextInputValue;

    nextInputValue = analogRead(pin);
    serialPrintf("stillMoving=%d nextInputValue=%d", stillMoving, nextInputValue);
    atMax = almostEqual(nextInputValue, maxInputValue, PETAL_SEEKING_TOLERANCE);

  } while (stillMoving && atMax);

  stopActuator();

  return stillMoving;
}

void seekToLight() {
  boolean stillMovable;
  int inputValue = analogRead(LIGHT_SENSOR_PIN);
  int initialValue = inputValue;
  if (almostEqual(inputValue, g_lastInputValue, STILL_TOLERANCE)) {
    return;
  }
  stillMovable = seekHighInput(LIGHT_SENSOR_PIN, g_currentDirection, inputValue);
  inputValue = analogRead(LIGHT_SENSOR_PIN);
  boolean lessThanAtStart = inputValue < initialValue - STILL_TOLERANCE;
  if (!stillMovable || lessThanAtStart ) {
    changeDirection();
  }
  // if we are receiving less input than at start of this call, changeDirection
  // and go back to max
  if (lessThanAtStart) {
    seekHighInput(LIGHT_SENSOR_PIN, g_currentDirection, inputValue);
  }
  g_lastInputValue = inputValue;
}


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(ACTUATOR_PWM_PIN, OUTPUT);
  pinMode(ACTUATOR_DIR_PIN, OUTPUT);

  pinMode(ACTUATOR_AMP_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  //  we really don't need to calibrate anything, the seek algorithm should handle
  //  variance in the ranges of inputs to the light sensor.
  //
  //  the scan of the range of movement for highest input source that calibrate()
  //  performs may come in handy in high ambient light environments where it's hard
  //  to distinguish a singular source.  The seek algorithm may need to be iterated
  //  on to better support high ambient light environments
  //
  //calibrate();

  serialPrintf("setup complete");
}

// the loop routine runs over and over again forever:
void loop() {
  seekToLight();
}
