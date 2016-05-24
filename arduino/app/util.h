
#ifndef __UTIL_INCLUDED
#define __UTIL_INCLUDED

void serialPrintf(char const *format, ...);
bool almostEqual(int a, int b, int tolerance);
int freeRam();

#endif
