
#include "animation.h"
#include "../util.h"

Animation::Animation(Petal *pPetal,  Adafruit_WS2801 *pStrip) 
  : _pPetal(pPetal), _pStrip(pStrip){

  // serialPrintf("Animation constructor pPetal=%p _pPetal=%p pStrip=%p _pStrip=%p", 
  //   pPetal, _pPetal, pStrip, _pStrip);
  // 
  _lastLoopThrottledCall = 0;
  
}


boolean Animation::_throttleLoop(unsigned long delay) {
  unsigned long thisTick = millis();
  if( thisTick - _lastLoopThrottledCall > delay ){
    _lastLoopThrottledCall = thisTick;
    return false;
  }
  return true;
    
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
  // serialPrintf("setColor _pPetal=0x%08x _pStrip=0x%08x  c=0x%08x numPixels=%d", 
  //   _pPetal, _pStrip, c, _pStrip->numPixels()); 

  for (unsigned int i = 0; i < _pStrip->numPixels(); i++) {
    _pStrip->setPixelColor(i, c);
  } 
  _pStrip->show();
  
  
}
