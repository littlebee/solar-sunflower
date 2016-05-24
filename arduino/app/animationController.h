
#ifndef __AnimationController_INCLUDED
#define __AnimationController_INCLUDED

#include "SPI.h"
#include "Adafruit_WS2801.h"

#include "petal.h"
#include "animations/animation.h"
#include "animations/colorTest.h"

class AnimationController {
  
public: 
  AnimationController(Petal *pPetal, int clockPin=2, int dataPin=3, int numLeds=25);
  
  void setup();
  
  boolean isAnimating();
  // start animating
  void animate(int animationIndex = 0);
  // stop animating 
  void halt();    
  // should be called once for each arduino loop 
  void loop();
  
  
private:
  Petal *_pPetal;
  short _clockPin, _dataPin, _numLeds;
  
};

#endif // __AnimationController_INCLUDED
