/*
  This script moves an actuator until maximum analog input is
  acheived.  While it's example application is to track a light source
  with a solar cell that is attached to the end of the actuator, it
  could be used to track maximum input of anything attached to the
  LIGHT_SENSOR_PIN.  maybe like sound source tracking too?  :)

*/
#define SERIALCOMMAND_HARDWAREONLY 1

#include <Arduino.h>
#include <SerialCommand.h>

#include "util.h"
#include "petal.h"

SerialCommand scm;
Petal petal;

void onCalibrate() {
  petal.calibrate();
  petal.printStatus();
}

void onSeek() {
  petal.seek();
  petal.printStatus();
}

void onHalt() {
  petal.halt();
  petal.printStatus();
}

void onStatus() {
  petal.printStatus();
}

void onExtend() {
  petal.moveToEnd(1);
  petal.printStatus();
}

void onRetract() {
  petal.moveToEnd(0);
  petal.printStatus();
}

void onUnrecognized() {
  serialPrintf("ERROR unrecognized command");
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  petal.setup();
  
  // Setup callbacks for SerialCommand commands 
  scm.addCommand("calibrate", onCalibrate);       
  scm.addCommand("seek", onSeek);       
  scm.addCommand("halt", onHalt);  
  scm.addCommand("status", onStatus);
  scm.addCommand("extend", onExtend);
  scm.addCommand("retract", onRetract);
  scm.addDefaultHandler(onUnrecognized);  
  
  Serial.println("Ready");   
}

// the loop routine runs over and over again forever:
void loop() {
  scm.readSerial(); 
  petal.loop();
}
