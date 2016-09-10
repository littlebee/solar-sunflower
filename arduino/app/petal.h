
#ifndef __PETAL_INCLUDED
#define __PETAL_INCLUDED

#include <Arduino.h>
#include "animationController.h"

enum PetalStates { PETAL_HALTED, PETAL_CALIBRATING, PETAL_SEEKING, PETAL_MOVING };
enum CalibrationStages { NOT_CALIBRATING, CALIBRATION_RESETING, CALIBRATION_SAMPLING, CALIBRATION_POSITIONING };

class Petal {
  
public: 
  Petal();
  void setup();
  
  bool isMoving();
  
  // should be called once for each arduino loop 
  void loop();
  
  void halt();
  bool isHalted();
  
  // This method takes the petal from wherever it is and extends it (closes)
  // full and then opens it fully and computes to time end to end and the point
  // of highest power input.  This method is synchronous.
  void calibrate();
  
  // This is the main method to start the petal into power seek mode
  // This method is asynchronous. you must call our loop() method periodically
  // after calling seek()
  void seek();
  
  // returns the current position of the actuator in milliseconds 
  // where 0 = fully retracted and ~30000 = fully extended
  unsigned int getCurrentPosition();
  
  // Prints a one line status message to serial out
  void printStatus();
  
  // when on, printStatus is called once every interval milliseconds. 
  // pass 0 to stop streaming
  void stream(unsigned long interval=1000){ _streamingInterval = interval; };
  
  // returns true if streaming
  bool isStreaming(){ return _streamingInterval > 0; };

  void moveToEnd(byte direction);

  // move to absolute position where mseconds is the time from fully
  // retracted moving out
  bool moveToPosition(unsigned int mSeconds=-1);

  // see animationController.cpp for list of animations
  void animate(int animationIndex = 0);  
  
private:
  byte _petalId;
  byte _direction;
  int _lastLightSensorValue;
  int _calibratedHighLightSensorValue;
  int _actualHighLightSensorValue;
  unsigned int _calibratedHighLightSensorMs;
  unsigned int _actualHighLightSensorMs;
  unsigned int _calibratedDurationMs;
  unsigned int _calibrationSamplingStartedMs;
  unsigned int _positionMs;
  unsigned int _requestedPositionMs;
  unsigned long _actuatorStartedAt;
  PetalStates _petalState;
  CalibrationStages _calibrationStage; 
  unsigned long _streamingInterval;
  unsigned long _lastStreamedAt;
  AnimationController *_animationController;
  
  void calibrateLoop();
  void moveLoop();
  void seekLoop();
  void streamLoop();
  
  void stopActuator();
  void startActuator(byte direction);

  void changeDirection(byte direction=-1);
  bool seekHighInput(int pin, byte direction, int inputValue);
  
  
  
  
};

#endif //__PETAL_INCLUDED
