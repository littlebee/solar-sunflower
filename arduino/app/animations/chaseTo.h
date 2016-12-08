
#include "animation.h"

/*
  This class a port of the rainbowCycle() from standtest example of 
  https://github.com/adafruit/Adafruit-WS2801-Library

*/

class ChaseTo : public Animation {
  
public:
  ChaseTo(Adafruit_WS2801 *pStrip, unsigned long delay=100);
  
  virtual void reset();
  virtual void loop();

protected:
  int _currentPixel;
  
};
