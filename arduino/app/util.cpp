// limited to 255 char strings

#include <Arduino.h>

char serialPrintfBuffer[512];

void serialPrintf(char const *format, ...) {
  va_list args;
  va_start(args, format);
  vsprintf(serialPrintfBuffer, format, args);
  va_end(args);
  Serial.println(serialPrintfBuffer);
}

bool almostEqual(int a, int b, int tolerance) {
  return a >= b - tolerance && a <= b + tolerance;
}


int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

// returns true if it has not been at least delay millis since last call 
bool throttle(unsigned long delay, unsigned long &_lastCall)  {
  unsigned long thisTick = millis();
  // thisTick > _lastCall is either an error or integer wrap around
  //serialPrintf("throttle %ld %ld %ld", delay, _lastCall, thisTick);
  if (thisTick - _lastCall > delay || thisTick < _lastCall) {
    _lastCall = thisTick;
    return false;
  }
  return true;
}
