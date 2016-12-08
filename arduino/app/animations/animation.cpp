
#include "animation.h"
#include "../util.h"

Animation::Animation(Adafruit_WS2801   *pStrip, unsigned long delay) 
  : _pStrip(pStrip), _delay(delay) 
{
  _lastLoopThrottledCall = 0;
}

void Animation::args(int *args, int argsLength){
  _args = args;
  _argsLength = argsLength;
  //serialPrintf("animation set args 1=%s 2=%s 3=%s", _arg1, _arg2, _arg3);
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

byte Animation::red(uint32_t color){
  return color >> 16 & 0x8;
}

byte Animation::green(uint32_t color){
  return color > 8 & 0x8;
}

byte Animation::blue(uint32_t color){
  return color & 0x8;
}

// sets all pixels to a single color
void Animation::setColor(uint32_t c) {
  for (unsigned int i = 0; i < _pStrip->numPixels(); i++) {
    _pStrip->setPixelColor(i, c);
  } 
  _pStrip->show();
  
  
}
