
#ifndef __PETAL_INCLUDED
#define __PETAL_INCLUDED

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
  void stopActuator();
  
  // This method takes the petal from wherever it is and extends it (closes)
  // full and then opens it fully and computes to time end to end and the point
  // of highest power input.  This method is synchronous.
  void calibrate();
  
  // This is the main method to start the petal into power seek mode
  // This method is asynchronous. you must call our loop() method periodically
  // after calling seek()
  void seek();
  
  // Prints a one line status message to serial out
  void printStatus();
  
  // when on, printStatus is called once every interval milliseconds. 
  // pass 0 to stop streaming
  void stream(unsigned long interval=1000){ _streamingInterval = interval; };
  
  // returns true if streaming
  bool isStreaming(){ return _streamingInterval > 0; };

  void moveToEnd(int direction);
  
  
private:
  int _petalId;
  int _direction;
  int _lastLightSensorValue;
  int _calibratedHighLightSensorValue;
  unsigned long _calibratedHighLightSensorMs;
  int _actualHighLightSensorValue;
  unsigned long _actualHighLightSensorMs;
  unsigned long _calibratedDurationMs;
  unsigned long _calibrationSamplingStartedMs;
  PetalStates _petalState;
  CalibrationStages _calibrationStage; 
  unsigned long _streamingInterval;
  unsigned long _lastStreamedAt;
  
  void calibrateLoop();
  void moveToEndLoop();
  void seekLoop();
  void streamLoop();
  
  
  bool moveRelativeToCurrent(int direction, int mSeconds=0);

  void changeDirection(int direction=-1);
  bool seekHighInput(int pin, int direction, int inputValue);
  
  
  
  
};

#endif //__PETAL_INCLUDED
