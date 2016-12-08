
#include <Arduino.h>
#include "chaseTo.h"
#include "../util.h"



ChaseTo::ChaseTo(Adafruit_WS2801 *pStrip, unsigned long delay) 
  : Animation(pStrip, delay) 
{
  reset();
}

void ChaseTo::reset(){
  _currentPixel = 0;
}

void ChaseTo::loop(){
  if( _throttleLoop(30) ){
    return;
  }
  //serialPrintf("target red=%d green=%d blue=%d", _targetRed, _targetGreen, _targetBlue);
  _pStrip->setPixelColor(_currentPixel, color(_args[0], _args[1], _args[2]));
  _pStrip->show();
  
  if( ++_currentPixel >= _pStrip->numPixels() ){
    setColor(color(0, 0, 0));
    _currentPixel = 0;
  }

}
