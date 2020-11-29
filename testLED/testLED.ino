#include <FastLED.h>
CRGB leds[64];

void setup() {
  // put your setup code here, to run once:
  FastLED.setBrightness(100);
  FastLED.addLeds<WS2812, 13, GRB>(leds, 64);
  fill_solid(leds, 64, CRGB::Grey);
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
