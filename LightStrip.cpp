#include "LightStrip.h"

#define LED_PIN     13
#define LED_COUNT  60

LightStrip::LightStrip(uint8_t brightness) {
  // constructor implementation
 strip = new Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
 this->brightness = brightness;
}

void LightStrip::begin() {
  strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip->show();            // Turn OFF all pixels ASAP
  strip->setBrightness(this->brightness);
}

void LightStrip::update() {
  strip->setBrightness(this->brightness);
}

void LightStrip::pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip->fill(strip->Color(255, 100, 0, strip->gamma8(j)));
    strip->show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip->fill(strip->Color(255, 100, 0, strip->gamma8(j)));
    strip->show();
    delay(wait);
  }
}

LightStrip::~LightStrip() { delete strip; }