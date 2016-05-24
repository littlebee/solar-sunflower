

#include "animationController.h"
#include "animations/rainbowCycle.h"
#include "animations/colorTest.h"
#include "util.h"


#define LED_DATA_PIN 2     // yellow wire
#define LED_CLOCK_PIN 3    // green wire


Adafruit_WS2801 strip = Adafruit_WS2801(25, 2, 3);
Animation *_animations[16];
int _currentAnimationIndex;    // -1 when not animating


AnimationController::AnimationController(Petal *pPetal, int clockPin, int dataPin, int numLeds) 
  : _pPetal(pPetal) 
{ 
  //Serial.println("in AnimationController constructor");
  // serialPrintf("AnimationController::constructor(%p, %d, %d, %d)", pPetal, clockPin, dataPin, numLeds);
  // serialPrintf("AnimationController _strip.numPixels()=%d", _strip.numPixels());
  _animations[0] = new ColorTest(_pPetal, &strip);
  // _animations[1] = new RainbowCycle(pPetal, _strip);
  
  // initially no animation until asked
  _currentAnimationIndex = -1;     

}

void AnimationController::setup() {
  // _strip.begin();
  // _strip.show();
  strip.begin();
  strip.show();
}

// should be called once for each arduino loop 
void AnimationController::loop() {
  // Serial.println("AnimationController::loop()");
  if( isAnimating() ){
    _animations[_currentAnimationIndex]->loop();
  }
}

boolean AnimationController::isAnimating() {
  return _currentAnimationIndex != -1;
}

// start animating
void AnimationController::animate(int animationIndex){
  // if( _currentAnimationIndex != animationIndex )
  //   _animations[animationIndex]->reset();
  
  _currentAnimationIndex = animationIndex;
}

// stop animating 
void AnimationController::halt(){
  _currentAnimationIndex = -1;
}

