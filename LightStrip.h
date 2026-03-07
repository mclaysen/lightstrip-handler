#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <Adafruit_NeoPixel.h>

class LightStrip {
private:
  Adafruit_NeoPixel* strip;
  uint8_t brightness;
  uint32_t offSetting;

public:
  LightStrip(uint8_t brightness = 50);
  ~LightStrip(); 
  void begin();
  void update();
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
  void pulseWhite(uint8_t wait);
  void turnOff();
  void setBrightness(uint8_t brightness);
  // other method declarations...
};

#endif