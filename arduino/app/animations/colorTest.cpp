
#include <Arduino.h>
#include "colorTest.h"
#include "../util.h"


const unsigned short NUM_COLORS = 5;
const byte COLORS[NUM_COLORS][3] = {
  {255, 0,   0    },
  {0,   255, 0    },
  {0,   0,   255  },
  {255, 255, 255  },
  {0,   0,   0    }
};

ColorTest::ColorTest(Petal *pPetal, Adafruit_WS2801 *pStrip, unsigned long delay) 
  : Animation(pPetal, pStrip), _delay(delay) 
{
  reset();
}

void ColorTest::reset(){
  _currentColorIndex = 0;
  
}

void ColorTest::loop(){
  if( _throttleLoop(_delay) ){
    return;
  }
  setColor(color(
    COLORS[_currentColorIndex][0], 
    COLORS[_currentColorIndex][1], 
    COLORS[_currentColorIndex][2]
  ));
  
  if( ++_currentColorIndex >= NUM_COLORS )
    _currentColorIndex = 0;
    
}



