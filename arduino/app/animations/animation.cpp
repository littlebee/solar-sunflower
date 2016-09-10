
#include "animation.h"
#include "../util.h"

Animation::Animation(Adafruit_WS2801   *pStrip, unsigned long delay) 
  : _pStrip(pStrip), _delay(delay) 
{
  _lastLoopThrottledCall = 0;
}


boolean Animation::_throttleLoop(unsigned long delay) {
  return throttle(delay, _lastLoopThrottledCall);
}

uint32_t Animation::color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

// sets all pixels to a single color
void Animation::setColor(uint32_t c) {
  for (unsigned int i = 0; i < _pStrip->numPixels(); i++) {
    _pStrip->setPixelColor(i, c);
  } 
  _pStrip->show();
  
  
}
