/*
  Every few minutes, move the actuator through it's full range of motion and report time,
  limit detection and other useful diagnostic info

  Tested on Arduino Uno with Cytron MD10 10A motor shield driving a Figerelli 30"/400lb
  actuator.

  The larger actuators have built in limits that stop the motor, but no way of telling
  the controller that the actuator is at it's limit.   Using an ACS712 hall effect sensor
  module connected to the ACTUATOR_AMP_SENSOR_PIN, we detect when the actuator should be
  moving but the motor is not drawing any power.

*/

#include <Arduino.h>

#define ACTUATOR_AMP_SENSOR_PIN A5
#define ACTUATOR_PWM_PIN 11
#define ACTUATOR_DIR_PIN 12

// MAX speed is 255 - this is the value that gets written to the ACTUATOR_PWM_PIN
#define ACTUATOR_SPEED  255

// when the amp sensor analog value is between these, it is considered to be at rest (no
// current being drawn.
#define ZEROAMP_RANGE_LOW 505
#define ZEROAMP_RANGE_HIGH 520


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
  serialPrintf("Amp sensor reading: %d", val);
  return val < ZEROAMP_RANGE_LOW || val > ZEROAMP_RANGE_HIGH;
}

void moveActuator(int direction, int speed=ACTUATOR_SPEED) {
  serialPrintf("moving actuator direction=%d, speed=%d", direction, speed);
  analogWrite(ACTUATOR_PWM_PIN, speed);
  digitalWrite(ACTUATOR_DIR_PIN, direction);
  delay(200);
}

void stopActuator() {
  analogWrite(ACTUATOR_PWM_PIN, 0);
}

void someBogusMethod() {
  /* just testing to see what atom does with this and how the git integration works */

}

// returns the number of milleseconds taken
unsigned long moveActuatorToEnd(int direction) {
  long startMs = millis();
  moveActuator(direction);
  delay(100);
  //while (millis() - startMs < 2000) {
  //  delay(1);
  //}
  while (isActuatorMoving()){
    delay(300);  // slight delay between sampling the ACS712 module is recommended
  }
  stopActuator();
  long endMs = millis();
  return endMs - startMs;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(ACTUATOR_PWM_PIN, OUTPUT);
  pinMode(ACTUATOR_DIR_PIN, OUTPUT);

  pinMode(ACTUATOR_AMP_SENSOR_PIN, INPUT);

  serialPrintf("setup complete");

}

// the loop routine runs over and over again forever:
void loop() {
  int currentDirection = 0;

  if (isActuatorMoving()) {
    serialPrintf("FAILURE: sensing movement on actuator when it should not be");
    delay(250);
    return;
  }
  // start at one end it should already be at an end unless the actuator started the loop
  // in the middle.   We could test that it takes less than n time to the first end on
  // second and subsequent iterations of loop()
  moveActuatorToEnd(currentDirection);
  delay(2000);

  for( int i = 0; i < 2; i++ ){
    currentDirection = (currentDirection + 1) % 2;   // 0 -> 1, 1 -> 0
    long ttm = moveActuatorToEnd(currentDirection);
    serialPrintf("moved end to end (%d) in %ldms", currentDirection, ttm);
    delay(1000);
  }

  delay(10000);

}
