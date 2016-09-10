
#include <Arduino.h>
#include "rainbowCycle.h"
#include "../util.h"

#define NUM_CYCLES 256



RainbowCycle::RainbowCycle(Adafruit_WS2801 *pStrip, unsigned long delay) 
  : Animation(pStrip, delay) 
{
  reset();
}

void RainbowCycle::reset(){
  _currentPixel = 0;
  _currentWheelCycle = 0;
}

void RainbowCycle::loop(){
  // serialPrintf("rainbowCycle::loop _delay=%ld", _delay);
  int numPixels = _pStrip->numPixels();
  int next = (_currentWheelCycle * NUM_CYCLES / numPixels + _currentPixel);
  uint32_t color = wheel(next % NUM_CYCLES);
  // serialPrintf("setting pixel %d to %X", _currentPixel, color);
  _pStrip->setPixelColor(_currentPixel, color);
  _pStrip->show();
  
  if( ++_currentWheelCycle > NUM_CYCLES ) 
    _currentWheelCycle = 0;
  if( ++_currentPixel > numPixels )
    _currentPixel = 0;
}


//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t RainbowCycle::wheel(byte wheelPos)
{
  if (wheelPos < 85) {
   return color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if (wheelPos < 170) {
   wheelPos -= 85;
   return color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
   wheelPos -= 170; 
   return color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

