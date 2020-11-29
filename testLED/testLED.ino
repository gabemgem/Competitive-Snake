#include <FastLED.h>
CRGB leds[56];

void setup() {
  // put your setup code here, to run once:
  FastLED.setBrightness(100);
  FastLED.addLeds<WS2812, 13, GRB>(leds, 56);
  fill_solid(leds, 56, CRGB::Grey);
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
