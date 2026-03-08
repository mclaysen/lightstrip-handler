#include "LightStrip.h"

// Pin 13 is SPI SCK on many Arduino boards and conflicts with Ethernet shields.
// Use a non-SPI GPIO for LED data.
#define LED_PIN     9
#define LED_COUNT  60

// Time scaling factors for each component
#define TIME_FACTOR_HUE 60
#define TIME_FACTOR_SAT 100
#define TIME_FACTOR_VAL 100

Rgbw rgbw = Rgbw(
    kRGBWDefaultColorTemp,
    kRGBWExactColors,      // Mode
    W3                     // W-placement
);

typedef WS2812<LED_PIN, RGB> ControllerT;  // RGB mode must be RGB, no re-ordering allowed.
static RGBWEmulatedController<ControllerT, GRB> rgbwEmu(rgbw);  // ordering goes here.


LightStrip::LightStrip(uint8_t brightness) {
  // constructor implementation
 strip = new CRGB[LED_COUNT];
 this->brightness = brightness;
 this->offSetting = CRGB::Black;
}

void LightStrip::begin() {
  // Emulate RGBW output exactly like the reference example, but on this class buffer.
  FastLED.addLeds(&rgbwEmu, strip, LED_COUNT);
  FastLED.setBrightness(this->brightness);
  delay(2000);
}

void LightStrip::update() {
  FastLED.setBrightness(this->brightness);
}

void LightStrip::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  uint32_t ms = millis();

  for(int i=0; i<LED_COUNT; i++) {
    // Use different noise functions for each LED and each color component
    uint8_t hue = inoise16(ms * TIME_FACTOR_HUE, i * 1000, 0) >> 8;
    uint8_t sat = inoise16(ms * TIME_FACTOR_SAT, i * 2000, 1000) >> 8;
    uint8_t val = inoise16(ms * TIME_FACTOR_VAL, i * 3000, 2000) >> 8;
    
    // Map the noise to full range for saturation and value
    sat = map(sat, 0, 255, 30, 255);
    val = map(val, 0, 255, 100, 255);
    
    strip[i] = CHSV(hue, sat, val);
  }
  FastLED.show();
}

void LightStrip::setBrightness(uint8_t brightness) {
  this->brightness = brightness;
  FastLED.setBrightness(this->brightness);
}

void LightStrip::pulseWhite(uint8_t wait) {
    static size_t frame_count = 0;
    int frame_cycle = frame_count % 4;
    frame_count++;

    CRGB pixel;
    switch (frame_cycle) {
        case 0:
            pixel = CRGB::Red;
            break;
        case 1:
            pixel = CRGB::Green;
            break;
        case 2:
            pixel = CRGB::Blue;
            break;
        case 3:
            pixel = CRGB::White;
            break;
    }

    for (int i = -1; i < frame_cycle; ++i) {
        fillAndShow(pixel);
        delay(200);
        fillAndShow(CRGB::Black);
        delay(200);
    }
    delay(1000);
}

void LightStrip::fillAndShow(CRGB color) {
  for(int i=0; i<LED_COUNT; i++) {
    strip[i] = color;
  }
  FastLED.show();
}

void LightStrip::turnOff() {
  fillAndShow(CRGB::Black);
}

void LightStrip::testSequential() {
    static size_t frame_count = 0;
    int frame_cycle = frame_count % 4;
    frame_count++;

    CRGB pixel;
    switch (frame_cycle) {
        case 0:
            pixel = CRGB::Red;
            break;
        case 1:
            pixel = CRGB::Green;
            break;
        case 2:
            pixel = CRGB::Blue;
            break;
        case 3:
            pixel = CRGB::White;
            break;
    }

    for (int i = -1; i < frame_cycle; ++i) {
        fillAndShow(pixel);
        delay(200);
        fillAndShow(CRGB::Black);
        delay(200);
    }
    delay(1000);
}

LightStrip::~LightStrip() { delete[] strip; }