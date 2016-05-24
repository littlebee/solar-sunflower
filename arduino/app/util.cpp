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

boolean almostEqual(int a, int b, int tolerance) {
  return a >= b - tolerance && a <= b + tolerance;
}


int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

