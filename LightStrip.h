#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <FastLED.h>

class LightStrip {
private:
  CRGB* strip;
  uint8_t brightness;
  uint32_t offSetting;
  void fillAndShow(CRGB color);

public:
  LightStrip(uint8_t brightness = 10);
  ~LightStrip(); 
  void begin();
  void update();
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
  void pulseWhite(uint8_t wait);
  void turnOff();
  void setBrightness(uint8_t brightness);
  void testSequential();
  // other method declarations...
};

#endif