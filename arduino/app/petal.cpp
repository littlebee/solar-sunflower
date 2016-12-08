\/*
  This script moves an actuator until maximum analog input is
  acheived.  While it's example application is to track a light source
  with a solar cell that is attached to the end of the actuator, it
  could be used to track maximum input of anything attached to the
  LIGHT_SENSOR_PIN.  maybe like sound source tracking too?  :)

*/

#include <Arduino.h>
#include "./libraries/ArduinoJson/include/ArduinoJson.h"

#include "petal.h"
#include "util.h"

#define ACTUATOR_AMP_SENSOR_PIN A2
#define ACTUATOR_PWM_PIN 11
#define ACTUATOR_DIR_PIN 12
#define LIGHT_SENSOR_PIN A5

// the actuators calibrated duration is between 26 and 27 seconds
#define MAX_DURATION_MS 30000
#define FULLY_EXTENDED MAX_DURATION_MS
#define FULLY_RETRACTED 0


// this is the difference to ignore in requests to move to an absolute
// or relative location. we cant really precisely position, because we
// have to yeild and the loop frequency might be longer than our diff in
// actual vs requested position
#define POSITIONAL_TOLERANCE 100 


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

static const char* PETAL_STATE_NAMES[] = {"HALTED", "CALIBRATING", "SEEKING", "MOVING"};
static int DIP_SWITCH_PINS[] = {4,5,6,7};



Petal::Petal() {
  _direction = EXTEND;
  _lastLightSensorValue = 0;
  _petalState = PETAL_HALTED;
  _streamingInterval = 0;
  _lastStreamedAt = 0;
  _petalId = 0;
  _calibratedHighLightSensorValue = 0;
  _calibratedHighLightSensorMs = 0;
  // default for the 30" actuator is based on 1 sec per inch
  _calibratedDurationMs = 30;       
  _positionMs = 0;
  _actuatorStartedAt = 0;
  _animationController = NULL;  // initialized in setup()
}

void Petal::setup() {
  pinMode(ACTUATOR_PWM_PIN, OUTPUT);
  pinMode(ACTUATOR_DIR_PIN, OUTPUT);

  pinMode(ACTUATOR_AMP_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  for (int i = 0; i < 4; i++){
    pinMode(DIP_SWITCH_PINS[i], INPUT);
    int pinValue = digitalRead(DIP_SWITCH_PINS[i]);
    _petalId = ((_petalId << 1) | pinValue);
  }
  
  if( _animationController != NULL ) 
    delete _animationController;
  _animationController = new AnimationController();
  _animationController->setup();

}

// using JSON may be a little overkill, but it allows you to 
// add something in here and not touch the API server to get it
// in the front end.  The JSON we emit here becomes the attributes
// of the petal model in the api which gets serialized to the 
// React front end.
void Petal::printStatus() {
  StaticJsonBuffer<200> jsonBuffer;
  
  JsonObject& root = jsonBuffer.createObject();
  root["petalId"] = _petalId;
  root["petalState"] = PETAL_STATE_NAMES[_petalState];
  root["position"] = getCurrentPosition();
  root["solarSensorValue"] = analogRead(LIGHT_SENSOR_PIN);
  root["actuatorSensorValue"] = analogRead(ACTUATOR_AMP_SENSOR_PIN);
  
  root["calibratedHighLightSensorValue"] = _calibratedHighLightSensorValue;
  root["calibratedHighLightSensorMs"] = _calibratedHighLightSensorMs;
  root["calibratedDurationMs"] = _calibratedDurationMs;
  root["animationIndex"] = _animationController->getAnimationIndex();
  root["freeRAM"] = freeRam();
  
  // TODO : maybe add some data logging and avg, min, max over time so the 
  //    api doesn't have to constantly call status?
  // JsonArray& data = root.createNestedArray("data");
  // data.add(48.756080, 6);  // 6 is the number of decimals to print
  // data.add(2.302038, 6);  // if not specified, 2 digits are printed
  
  root.printTo(Serial);
  Serial.println("");    // add newline 

}

boolean Petal::isMoving() {
  int val = analogRead(ACTUATOR_AMP_SENSOR_PIN);
  return val < ZEROAMP_RANGE_LOW || val > ZEROAMP_RANGE_HIGH;
}

void Petal::halt() {
  stopActuator();
  _petalState = PETAL_HALTED;
  _streamingInterval = 0;
}

boolean Petal::isHalted() {
  return _petalState == PETAL_HALTED;
}

void Petal::animate(int animationIndex, int *args, int argsLength) {
  _animationController->animate(animationIndex, args, argsLength);
}

void Petal::startActuator(byte direction) {
  digitalWrite(ACTUATOR_DIR_PIN, direction);
  analogWrite(ACTUATOR_PWM_PIN, ACTUATOR_SPEED);
  _direction = direction;
  _actuatorStartedAt = millis();
}

void Petal::stopActuator() {
  // stop the actuator if moving
  analogWrite(ACTUATOR_PWM_PIN, 0);
  unsigned int currentPosition = getCurrentPosition();
  _positionMs = min(max(currentPosition, 0), MAX_DURATION_MS);
  _actuatorStartedAt = 0;
}

unsigned int Petal::getCurrentPosition(){
  int neg = _direction == EXTEND ? 1 : -1;
  if( _actuatorStartedAt > 0 )
    return _positionMs + neg * (millis() - _actuatorStartedAt);
    
  return _positionMs;
}

void Petal::loop() {
  _animationController->loop();
  streamLoop();

  if (isHalted()){
    return;
  }
  if (_petalState == PETAL_SEEKING)
    seekLoop();
  else if (_petalState == PETAL_CALIBRATING)
    calibrateLoop();
  else if (_petalState == PETAL_MOVING) 
    moveLoop();
    
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

  if (_calibrationStage == CALIBRATION_RESETING) {
    if( !isMoving() ){
      moveToPosition(FULLY_RETRACTED);    // start moving
      delay(SAMPLING_DELAY);
    }
    if (!isMoving()) {        // should be moving unless at end
      _calibrationStage = CALIBRATION_SAMPLING;
      _calibrationSamplingStartedMs = millis();
    }
    return;
  }
  
  if (_calibrationStage == CALIBRATION_SAMPLING) {
    if (!isMoving()) {
      moveToPosition(FULLY_EXTENDED);
      delay(SAMPLING_DELAY);
    }
    if (isMoving() && !isHalted() ) {
      int inputValue = analogRead(LIGHT_SENSOR_PIN);
      if (inputValue > _calibratedHighLightSensorValue) {
        _calibratedHighLightSensorValue = inputValue;
        _calibratedHighLightSensorMs = millis() - _calibrationSamplingStartedMs;
      }
    } 
    else {
      long endMs = millis();
      _calibratedDurationMs = (endMs - _calibrationSamplingStartedMs);
      // for now leave it fully extended until next command
      // _calibrationStage = CALIBRATION_POSITIONING;
      _calibrationStage = NOT_CALIBRATING;
      halt();
      return;
    }
  }
  
  if (_calibrationStage == CALIBRATION_POSITIONING) {
    // move back to point in time when at max
    if( moveToPosition(_calibratedDurationMs - _calibratedHighLightSensorMs) ){
      _calibrationStage = NOT_CALIBRATING;
      halt();
    }
  }
}

void Petal::seek() {
  // the next call to loop will actually start the seeking
  _petalState = PETAL_SEEKING;
}

// moves a petal to an absolute position with fully retracted = 0
// and fully extended < 30000.   You must call this from a loop
// with the same value to have it stop at the requested position.
//
// returns false if petal is moving to position, true if within tolerance
// of requested position
boolean Petal::moveToPosition(unsigned int mSeconds /* =0 */) {
  if( mSeconds == -1 )
    mSeconds = _requestedPositionMs;
  else
    _requestedPositionMs = mSeconds;
  
  int direction = _positionMs < mSeconds ? EXTEND : RETRACT;
  int currentPosition = getCurrentPosition();
  boolean withinRange = 
    (direction == EXTEND && (currentPosition + POSITIONAL_TOLERANCE) >= mSeconds) ||
    (direction == RETRACT && (currentPosition - POSITIONAL_TOLERANCE) <= mSeconds);
    
    
  // moveToPosition is also called by the other states, don't change state unless halted
  if( _petalState == PETAL_HALTED ) _petalState = PETAL_MOVING;
  
  if( direction != _direction && isMoving() ){
    stopActuator();
    delay(SAMPLING_DELAY);
  }
  
  // # run to the end ignoring current calculated position
  if( mSeconds >= FULLY_EXTENDED || mSeconds <= FULLY_RETRACTED ){
    if( !isMoving() ) { 
      startActuator(direction);
      delay(SAMPLING_DELAY);
      if( !isMoving() ) {
        stopActuator(); 
        _positionMs = direction == EXTEND ? _calibratedDurationMs : 0;
        if( _petalState == PETAL_MOVING ) _petalState = PETAL_HALTED ;
      }
    }
  } 
  else if( withinRange ){
    if( isMoving() )
      stopActuator();
    
    return true;
  } 
  if( !isMoving() ){
    startActuator(direction);
  }
  
  return false;
}

// returns the number of milliseconds taken
void Petal::moveToEnd(byte direction) {
  int position = direction == EXTEND ? FULLY_EXTENDED : FULLY_RETRACTED;
  moveToPosition(position);
  _petalState = PETAL_MOVING;
  
}

void Petal::moveLoop() {
  if (isMoving() && !isHalted()){
    moveToPosition();
    return;
  }
  else {
    if (!isHalted()) halt();
  }
  delay(SAMPLING_DELAY);  // slight delay between sampling the ACS712 module is recommended
}

void Petal::changeDirection(byte direction /* =-1 */) {
  if (direction == -1)
    direction = _direction + 1;
  // mask to single bit - 0 or 1, so 0 + 1 becomes 1 and 1 + 1 becomes 0
  _direction = direction & 1;
}


// returns false if at end of travel
boolean Petal::seekHighInput(int pin, byte direction, int inputValue) {
  int nextInputValue = inputValue;
  int maxInputValue = inputValue;
  boolean stillMoving;
  boolean atMax;

  startActuator(direction);

  do {
    delay(500);
    if (isHalted()) break;
    stillMoving = isMoving();

    if (nextInputValue > maxInputValue)
      maxInputValue = nextInputValue;

    nextInputValue = analogRead(pin);
    atMax = almostEqual(nextInputValue, maxInputValue, PETAL_SEEKING_TOLERANCE);

  } while (stillMoving && atMax);

  stopActuator();

  return stillMoving;
}

void Petal::seekLoop() {
  boolean stillMovable;
  int inputValue = analogRead(LIGHT_SENSOR_PIN);
  int initialValue = inputValue;
  
  if (isHalted()) return;
  
  if (almostEqual(inputValue, _lastLightSensorValue, STILL_TOLERANCE)) {
    return;
  }
  stillMovable = seekHighInput(LIGHT_SENSOR_PIN, _direction, inputValue);
  inputValue = analogRead(LIGHT_SENSOR_PIN);
  boolean lessThanAtStart = inputValue < initialValue - STILL_TOLERANCE;
  if (!stillMovable || lessThanAtStart ) {
    changeDirection();
  }
  // if we are receiving less input than at start of this call, changeDirection
  // and go back to max
  if (lessThanAtStart) {
    seekHighInput(LIGHT_SENSOR_PIN, _direction, inputValue);
  }
  _lastLightSensorValue = inputValue;
}

void Petal::streamLoop() {
  //serialPrintf("streamLoop %ld", _streamingInterval);
  if (!isStreaming()) return;
  if (throttle(_streamingInterval, _lastStreamedAt)) return;
  printStatus();
  
}


