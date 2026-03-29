#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <FastLED.h>

class LightStrip {
private:
  CRGB* strip;
  void fillAndShow(CRGB color);

public:
  LightStrip(uint8_t brightness = 10);
  ~LightStrip(); 
  void begin();
  void update();
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
  void setWhite(uint8_t w);
  void turnOff();
  void setBrightness(uint8_t brightness);
  void setKelvin(uint16_t kelvin, uint8_t level);
};

#endif