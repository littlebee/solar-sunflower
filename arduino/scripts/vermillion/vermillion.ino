
#include <Adafruit_WS2801.h>

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
const int DATA_PIN = 2;       // 'yellow' wire
const int CLOCK_PIN = 3;      // 'green' wire

const int SENSOR_PIN = A0;

// highest component color value we want to push. could be as high as 31
const int MAX_COLOR = 31;
const int MIN_COLOR = 2;
const int NUM_PIXELS = 50;

const int INACTIVE_MAX_BLUE = MAX_COLOR / 2;
const int INACTIVE_MIN_BLUE = MIN_COLOR;
const int INACTIVE_MAX_GREEN = MAX_COLOR;
const int INACTIVE_MIN_GREEN = MAX_COLOR + 10;
const int INACTIVE_MAX_RED = 0;
const int INACTIVE_MIN_RED = 0;

const int ACTIVE_MAX_BLUE = 0;
const int ACTIVE_MIN_BLUE = 0;
const int ACTIVE_MAX_GREEN = 20;
const int ACTIVE_MIN_GREEN = 8;
const int ACTIVE_MAX_RED = MAX_COLOR;
const int ACTIVE_MIN_RED = MAX_COLOR;

const int LOOP_DELAY = 50;
// odds of any one pixel being full white for 1 loop delay
const int SPARKLE_DEMONINATOR = 100;



Adafruit_WS2801 strip = Adafruit_WS2801(NUM_PIXELS, DATA_PIN, CLOCK_PIN);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();

  pinMode(SENSOR_PIN, INPUT);

  Serial.begin(9600);
  Serial.println("Starting Up Vermillion animation...");  
}


void loop() {
  // put your main code here, to run repeatedly:
  int i;
  int component;
  int step;
  int red;
  int green;
  int blue;
  int sensorValue;
  
  // sensor values between 20 - 200 cause an equal percentage of the first and last
  // 20 pixels to go to red scale 
  sensorValue = analogRead(SENSOR_PIN);

  for (int i = 0; i < NUM_PIXELS; i++){
    if (random(SPARKLE_DEMONINATOR) == 1){
      red = MAX_COLOR;
      green = MAX_COLOR;
      blue = MAX_COLOR;
    } else if( shouldGoRed(i, sensorValue) ){
      red = random(ACTIVE_MIN_RED, ACTIVE_MAX_RED);
      green = random(ACTIVE_MIN_GREEN, ACTIVE_MAX_GREEN);
      blue = random(ACTIVE_MIN_BLUE, ACTIVE_MAX_BLUE);
    } else {
      red = random(INACTIVE_MIN_RED, INACTIVE_MAX_RED);
      green = random(INACTIVE_MIN_GREEN, INACTIVE_MAX_GREEN);
      blue = random(INACTIVE_MIN_BLUE, INACTIVE_MAX_BLUE);
    }
    strip.setPixelColor(i, red, green, blue);
  }
  strip.show();
  delay(LOOP_DELAY);
}

bool shouldGoRed(int pixelIndex, int sensorValue){
  char outputBuffer[64];
  int degree = (sensorValue / 9);
  
  if( pixelIndex > 20 && pixelIndex < 30 || sensorValue > 200)
    return false;

  // Serial.print(itoa(pixelIndex, outputBuffer, 10));
  // Serial.print("- SENSOR_PIN: ");
  // Serial.print(", degree: ");
  // Serial.print(itoa(degree, outputBuffer, 10));
  // Serial.print("\n");

  return pixelIndex < 20 - degree || pixelIndex > 30 && pixelIndex < 50 - degree;
}



