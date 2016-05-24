
#ifndef __colorTest_INCLUDED
#define __colorTest_INCLUDED

#include "animation.h"

/*

*/

class ColorTest : public Animation {
  
public:
  ColorTest(Petal *pPetal,  Adafruit_WS2801 *pStrip, unsigned long delay=2500);

  virtual void reset();
  virtual void loop();

protected:
  unsigned short _currentColorIndex;
  unsigned long _delay;
  
  
};

#endif // __colorTest_INCLUDED