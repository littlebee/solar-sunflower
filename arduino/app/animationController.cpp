

#include "animationController.h"
#include "animations/rainbowCycle.h"
#include "animations/colorTest.h"
#include "animations/chaseTo.h"
#include "animations/fadeTo.h"
#include "util.h"


#define LED_DATA_PIN 2     // yellow wire
#define LED_CLOCK_PIN 3    // green wire


Adafruit_WS2801 strip = Adafruit_WS2801(50, 2, 3);
Animation *_animations[16];
int _currentAnimationIndex;    // -1 when not animating


AnimationController::AnimationController(int clockPin, int dataPin, int numLeds) 
{ 
  // animation index zero is handled here. All black and stop animating
  _animations[0] = NULL;
  _animations[1] = new ColorTest(&strip, 2500);
  _animations[2] = new RainbowCycle(&strip, 200);
  _animations[3] = new ChaseTo(&strip, 200);
  //_animations[3] = new FadeTo(&strip, 200);
  
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

int AnimationController::getAnimationIndex() {
  return _currentAnimationIndex;
}

// start animating
void AnimationController::animate(int animationIndex, int *args, int argsLength){
  // if( _currentAnimationIndex != animationIndex )
  //   _animations[animationIndex]->reset();
  
  _currentAnimationIndex = animationIndex;
  _animations[_currentAnimationIndex]->args(args, argsLength);
}

// stop animating 
void AnimationController::halt(){
  _currentAnimationIndex = -1;
}

