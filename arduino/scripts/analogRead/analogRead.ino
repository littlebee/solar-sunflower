/*

analogRead  - reads an analog pin N times per second over N period and
  writes the min and max to the console

*/
#include <Arduino.h>

#define ANALOG_PIN A4
#define SAMPLE_INTERVAL 10    // every 10ms
#define SAMPLE_PERIOD   1000  // 1 sec


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(ANALOG_PIN, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  int min = 0xff;
  int max = 0;

  for (int i = 0; i < SAMPLE_PERIOD / SAMPLE_INTERVAL; i ++) {
      int sensorValue = analogRead(ANALOG_PIN);
      if (min == 0xff || sensorValue < min) min = sensorValue;
      if (sensorValue > max) max = sensorValue;
      delay(SAMPLE_INTERVAL);
  }
  char out[255];
  sprintf(out, "min=%d max=%d", min, max);
  Serial.println(out);
}
