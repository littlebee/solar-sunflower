
#include "animation.h"

/*
  This class a port of the rainbowCycle() from standtest example of 
  https://github.com/adafruit/Adafruit-WS2801-Library

*/

class FadeTo : public Animation {
  
public:
  FadeTo(Adafruit_WS2801 *pStrip, unsigned long delay=100);
  
  virtual void reset();
  virtual void loop();

protected:
  byte moveTowards(byte currentColorComponent, byte targetColorComponent);
  
  
};
