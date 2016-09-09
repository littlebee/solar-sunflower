

#include "animationController.h"
#include "animations/rainbowCycle.h"
#include "animations/colorTest.h"
#include "util.h"


#define LED_DATA_PIN 2     // yellow wire
#define LED_CLOCK_PIN 3    // green wire


Adafruit_WS2801 strip = Adafruit_WS2801(50, 2, 3);
Animation *_animations[16];
int _currentAnimationIndex;    // -1 when not animating


AnimationController::AnimationController(Petal *pPetal, int clockPin, int dataPin, int numLeds) 
  : _pPetal(pPetal) 
{ 
  // animation index zero is handled here. All black and stop animating
  _animations[0] = NULL;
  _animations[1] = new ColorTest(_pPetal, &strip);
  _animations[2] = new RainbowCycle(pPetal, &strip);
  
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
    if( _currentAnimationIndex == 0 )
    {
      for (unsigned int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
      } 
      strip.show();
      _currentAnimationIndex = -1;  
    }
    else
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

