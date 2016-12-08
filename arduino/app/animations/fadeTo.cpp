
#include <Arduino.h>
#include "fadeTo.h"
#include "../util.h"


FadeTo::FadeTo(Adafruit_WS2801 *pStrip, unsigned long delay) 
  : Animation(pStrip, delay) 
{
  reset();
}

void FadeTo::reset(){
  // nothing to do here yet
}

void FadeTo::loop(){
  
  byte targetRed = _args[0];
  byte targetGreen = _args[1];
  byte targetBlue = _args[2];

  // serialPrintf("fadeTo: currentRed=%d currentGreen=%d currentBlue=%d",
  //   targetRed, targetBlue, targetGreen); //currentRed, currentGreen, currentBlue);
  // 
  
  int numPixels = _pStrip->numPixels();
  for( int i = 0; i < numPixels; i++ ){
    uint32_t currentColor = _pStrip->getPixelColor(i);
    byte currentRed = this->red(currentColor);
    byte currentGreen = this->green(currentColor);
    byte currentBlue = this->blue(currentColor);
    
    byte newRed = moveTowards(currentRed, targetRed);
    byte newGreen = moveTowards(currentGreen, targetGreen);
    byte newBlue = moveTowards(currentBlue, targetBlue);
    
    _pStrip->setPixelColor(i, newRed, newGreen, newBlue);
  }
  _pStrip->show();

}

byte FadeTo::moveTowards(byte currentColorComponent, byte targetColorComponent){
  if( currentColorComponent == targetColorComponent )
    return currentColorComponent;
    
  int direction = 1;
  if( currentColorComponent > targetColorComponent )
    direction = -1;
    
  return currentColorComponent + direction;
  
}

