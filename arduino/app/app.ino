/*
  This is the main entry point for the arduino code of the Solar Sunflower Project
*/
#define SERIALCOMMAND_HARDWAREONLY 1

#include <Arduino.h>
#include <SerialCommand.h>

#include "util.h"
#include "petal.h"
#include "animationController.h"

SerialCommand scm;
Petal *ourPetal = NULL; // = new Petal();
AnimationController *animationController = NULL; // = new AnimationController();

void onCalibrate() {
  ourPetal->calibrate();
  ourPetal->printStatus();
}

void onSeek() {
  ourPetal->seek();
  ourPetal->printStatus();
}

void onHalt() {
  animationController->halt();
  ourPetal->halt();
  ourPetal->printStatus();
}

void onStatus() {
  ourPetal->printStatus();
}

void onExtend() {
  ourPetal->moveToEnd(1);
  ourPetal->printStatus();
}

void onRetract() {
  ourPetal->moveToEnd(0);
  ourPetal->printStatus();
}

void onAnimate() {
  char *arg = scm.next();    // Get the next argument from the SerialCommand object buffer
  unsigned int animationIndex = 0;
  if (arg != NULL )
    animationIndex = atoi(arg); 

  animationController->animate(animationIndex);
  ourPetal->printStatus();
  
}

// not specifying an interval toggles streaming on and off
void onStream() {
  char *arg = scm.next();    // Get the next argument from the SerialCommand object buffer
  unsigned long interval = 0;
  if (arg == NULL )
    interval = (ourPetal->isStreaming()) ? 0 : 250;
  else 
    interval = atol(arg); 
  
  ourPetal->stream(interval);
}

// not specifying an interval toggles streaming on and off
void onPosition() {
  char *arg = scm.next();    // Get the next argument from the SerialCommand object buffer
  unsigned int position = 0;
  if (arg != NULL ){
    position = atoi(arg); 
    ourPetal->moveToPosition(position);
  }
  return ourPetal->printStatus();
    
}

void onHelp() {
  SerialCommandCallback *commands = scm.getCommands();
  int numCommands = scm.getNumCommands();
  for (int i = 0; i < numCommands; i++ )
    Serial.println(commands[i].command);
}

void onUnrecognized() {
  serialPrintf("{'error': 'ERROR unrecognized command'}");
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  // Setup callbacks for SerialCommand commands 
  scm.addCommand("calibrate", onCalibrate);       
  scm.addCommand("seek", onSeek);       
  scm.addCommand("halt", onHalt);  
  scm.addCommand("status", onStatus);
  scm.addCommand("stream", onStream);
  scm.addCommand("extend", onExtend);
  scm.addCommand("retract", onRetract);
  scm.addCommand("position", onPosition);
  scm.addCommand("animate", onAnimate);
  
  scm.addCommand("help", onHelp);
  scm.addDefaultHandler(onUnrecognized);  
  
  
  if( ourPetal != NULL ) delete ourPetal;
  ourPetal = new Petal();
  ourPetal->setup();
  
  if( animationController != NULL ) delete animationController;
  animationController = new AnimationController(ourPetal);
  animationController->setup();

  Serial.println("ready");   
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  scm.readSerial(); 
  ourPetal->loop();
  animationController->loop();
  
  
}
