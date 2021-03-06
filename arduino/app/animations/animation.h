
#ifndef __animation_INCLUDED
#define __animation_INCLUDED

#include "SPI.h"
#include "Adafruit_WS2801.h"



/*
  This is the abstract base class for all animation sequences. Animation routines can
  animate pixel LEDs and can also animate the petal movements via the Petal object
  passed to the constructor
*/
class Animation {

public:
  Animation(Adafruit_WS2801 *pStrip, unsigned long delay=2500);

  // reset any animation state here.  Don't change the state of the strip 
  // or petal position here.
  virtual void reset() = 0;
  
  // called once for each arduino "loop" call. 
  //
  // If you need to delay between calls to loop, don't use delay().  Instead, 
  // return from loop() without doing anything until x amount of time has passed 
  // and then do the next step of your animation.
  virtual void loop() = 0;

  // returns 24bit color
  virtual uint32_t color(byte r, byte g, byte b);
  
  // sets all pixels to a single color
  virtual void setColor(uint32_t c);
  
  virtual void args(int *args, int argsLength);
  
  byte red(uint32_t color);
  byte green(uint32_t color);
  byte blue(uint32_t color);

  
  
protected: 
  Adafruit_WS2801 *_pStrip;
  unsigned long _delay;
  
  // call this method at the start of your loop call with a delay. 
  // if it returns true, return without doing anything
  boolean _throttleLoop(unsigned long delay);
  int *_args;
  int _argsLength;
  
  
private:
  unsigned long _lastLoopThrottledCall;
  
  
  

};
   
   
#endif // __animation_INCLUDED

