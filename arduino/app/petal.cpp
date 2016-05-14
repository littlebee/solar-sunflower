/*
  This script moves an actuator until maximum analog input is
  acheived.  While it's example application is to track a light source
  with a solar cell that is attached to the end of the actuator, it
  could be used to track maximum input of anything attached to the
  LIGHT_SENSOR_PIN.  maybe like sound source tracking too?  :)

*/

#include <Arduino.h>

#include "petal.h"
#include "util.h"

#define ACTUATOR_AMP_SENSOR_PIN A5
#define ACTUATOR_PWM_PIN 11
#define ACTUATOR_DIR_PIN 12
#define LIGHT_SENSOR_PIN A4

// MAX speed is 255 - this is the value that gets written
// to the ACTUATOR_PWM_PIN.  Note do not change or limit
// detection will not work
#define ACTUATOR_SPEED  255

// when the amp sensor analog value is between these, it is
// considered to be at rest (no current being drawn).
#define ZEROAMP_RANGE_LOW 505
#define ZEROAMP_RANGE_HIGH 520

// directions for the actuator (value gets set to ACTUATOR_DIR_PIN)
#define RETRACT 0
#define EXTEND 1

// don't move if sampled light input value is withing
//STILL_TOLERANCE of _lastLightSensorValue
#define STILL_TOLERANCE 2
// move actuator until drop from max is less than
#define PETAL_SEEKING_TOLERANCE 2

// minimum number of milliseconds delay between sampling the ACS712
// recommended in manufacturer sample source
#define SAMPLING_DELAY 1

static const char* PetalStateNames[] = {"HALTED", "CALIBRATING", "SEEKING"};


Petal::Petal() {
  _currentDirection = EXTEND;
  _lastLightSensorValue = 0;
  _petalState = PETAL_HALTED;

  pinMode(ACTUATOR_PWM_PIN, OUTPUT);
  pinMode(ACTUATOR_DIR_PIN, OUTPUT);

  pinMode(ACTUATOR_AMP_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
}

boolean Petal::isMoving() {
  int val = analogRead(ACTUATOR_AMP_SENSOR_PIN);
  // serialPrintf("Amp sensor reading: %d", val);
  return val < ZEROAMP_RANGE_LOW || val > ZEROAMP_RANGE_HIGH;
}

void Petal::halt() {
  stopActuator();
  _petalState = PETAL_HALTED;
  printStatus();
}

boolean Petal::isHalted() {
  return _petalState == PETAL_HALTED;
}

void Petal::stopActuator() {
  // stop the actuator if moving
  analogWrite(ACTUATOR_PWM_PIN, 0);
}

void Petal::loop() {
  if (isHalted()){
    return;
  }
  
  if (_petalState == PETAL_SEEKING)
    seekToLight();
  else if (_petalState == PETAL_CALIBRATING)
    calibrateLoop();
    
  return;
    
  
  
  
  
}
void Petal::calibrate() {
  _petalState = PETAL_CALIBRATING;
  _calibrationStage = CALIBRATION_RESETING;
  _calibratedHighLightSensorValue = 0;
  _calibratedHighLightSensorMs = 0;
  _calibratedDurationMs = 0;
  
  // the next call to loop will set us off by calling calibrateLoop below
  
}

// finds the max input by moving through the full range of motion
void Petal::calibrateLoop() {
  _petalState = PETAL_CALIBRATING;
  
  if (_calibrationStage == CALIBRATION_RESETING) {
    if( !isMoving() )
      moveRelativeToCurrent(EXTEND);    // start moving
      
    if (!isMoving()) {        // should be moving unless at end
      _calibrationStage = CALIBRATION_SAMPLING;
      _calibrationSamplingStartedMs = millis();
    }
    return;
  }
  
  if (_calibrationStage == CALIBRATION_SAMPLING) {
    if (!isMoving()) {
      moveRelativeToCurrent(RETRACT);
    }
    if (isMoving() && !isHalted() ) {
      int inputValue = analogRead(LIGHT_SENSOR_PIN);
      if (inputValue > _calibratedHighLightSensorValue) {
        _calibratedHighLightSensorValue = inputValue;
        _calibratedHighLightSensorMs = millis() - _calibrationSamplingStartedMs;
        printStatus();
        delay(SAMPLING_DELAY);
      }
    } 
    else {
      long endMs = millis();
      _calibratedDurationMs = (endMs - _calibrationSamplingStartedMs);
      _calibrationStage = CALIBRATION_POSITIONING;
      return;
    }
  }
  
  if (_calibrationStage == CALIBRATION_POSITIONING) {
    // move back to point in time when at max
    moveRelativeToCurrent(EXTEND, _calibratedDurationMs - _calibratedHighLightSensorMs);
    _calibrationStage = NOT_CALIBRATING;
    _petalState = PETAL_HALTED;
  }
}

void Petal::seek() {
  // the next call to loop will actually start the seeking
  _petalState = PETAL_SEEKING;
}


void Petal::printStatus() {
  serialPrintf("%s %d %d %d %ld %ld", 
    PetalStateNames[_petalState], 
    _currentDirection, 
    _lastLightSensorValue,
    _calibratedHighLightSensorValue,
    _calibratedHighLightSensorMs,
    _calibratedDurationMs
  );
}


// mSeconds = optionally move in direction for n milliseconds and then stop
// returns false if mSeconds != 0 and at end of travel 
boolean Petal::moveRelativeToCurrent(int direction, int mSeconds /* =0 */) {
  analogWrite(ACTUATOR_PWM_PIN, ACTUATOR_SPEED);
  digitalWrite(ACTUATOR_DIR_PIN, direction);
  boolean stillMoving = isMoving();
  for (int i = 0; i < abs(mSeconds) / SAMPLING_DELAY && stillMoving; i++) {
    if (isHalted()) break;
    delay(SAMPLING_DELAY);
    stillMoving = isMoving();
  }
  stopActuator();
  return true;
}

// returns the number of milliseconds taken
unsigned long Petal::moveToEnd(int direction) {
  long startMs = millis();
  moveRelativeToCurrent(direction);
  while (isMoving()){
    if (isHalted()) break;
    delay(SAMPLING_DELAY);  // slight delay between sampling the ACS712 module is recommended
  }
  stopActuator();
  long endMs = millis();
  return endMs - startMs;
}

void Petal::changeDirection(int direction /* =-1 */) {
  if (direction == -1)
    direction = _currentDirection + 1;
  // mask to single bit - 0 or 1, so 0 + 1 becomes 1 and 1 + 1 becomes 0
  _currentDirection = direction & 1;
}


// returns false if at end of travel
boolean Petal::seekHighInput(int pin, int direction, int inputValue) {
  int nextInputValue = inputValue;
  int maxInputValue = inputValue;
  boolean stillMoving;
  boolean atMax;

  moveRelativeToCurrent(direction);

  do {
    delay(500);
    if (isHalted()) break;
    stillMoving = isMoving();

    if (nextInputValue > maxInputValue)
      maxInputValue = nextInputValue;

    nextInputValue = analogRead(pin);
    serialPrintf("stillMoving=%d nextInputValue=%d", stillMoving, nextInputValue);
    atMax = almostEqual(nextInputValue, maxInputValue, PETAL_SEEKING_TOLERANCE);

  } while (stillMoving && atMax);

  stopActuator();

  return stillMoving;
}

void Petal::seekToLight() {
  boolean stillMovable;
  int inputValue = analogRead(LIGHT_SENSOR_PIN);
  int initialValue = inputValue;
  
  if (isHalted()) return;
  
  if (almostEqual(inputValue, _lastLightSensorValue, STILL_TOLERANCE)) {
    return;
  }
  stillMovable = seekHighInput(LIGHT_SENSOR_PIN, _currentDirection, inputValue);
  inputValue = analogRead(LIGHT_SENSOR_PIN);
  boolean lessThanAtStart = inputValue < initialValue - STILL_TOLERANCE;
  if (!stillMovable || lessThanAtStart ) {
    changeDirection();
  }
  // if we are receiving less input than at start of this call, changeDirection
  // and go back to max
  if (lessThanAtStart) {
    seekHighInput(LIGHT_SENSOR_PIN, _currentDirection, inputValue);
  }
  _lastLightSensorValue = inputValue;
}



