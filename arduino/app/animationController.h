
#ifndef __AnimationController_INCLUDED
#define __AnimationController_INCLUDED

#include "SPI.h"
#include "Adafruit_WS2801.h"

#include "animations/animation.h"
#include "animations/colorTest.h"

class AnimationController {
  
public: 
  AnimationController(int clockPin=2, int dataPin=3, int numLeds=50);
  
  void setup();
  
  boolean isAnimating();
  // start animating
  void animate(int animationIndex = 0);
  // stop animating 
  void halt();    
  // should be called once for each arduino loop 
  void loop();
  
  int getAnimationIndex();
  
  
private:
  short _clockPin, _dataPin, _numLeds;
  
};

#endif // __AnimationController_INCLUDED
