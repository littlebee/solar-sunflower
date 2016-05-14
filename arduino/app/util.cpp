// limited to 255 char strings

#include <Arduino.h>

void serialPrintf(char const *format, ...) {
  char buffer[255];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  Serial.println(buffer);
}

boolean almostEqual(int a, int b, int tolerance) {
  return a >= b - tolerance && a <= b + tolerance;
}
