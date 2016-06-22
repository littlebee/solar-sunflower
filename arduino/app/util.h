
#ifndef __UTIL_INCLUDED
#define __UTIL_INCLUDED

void serialPrintf(char const *format, ...);
bool almostEqual(int a, int b, int tolerance);
int freeRam();
bool throttle(unsigned long delay, unsigned long &_lastCall);

#endif
