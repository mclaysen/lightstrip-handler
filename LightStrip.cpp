#include "LightStrip.h"
#include <math.h>
using namespace std;

#define LED_PIN     9
#define LED_COUNT  60

Rgbw rgbw = Rgbw(
    kRGBWDefaultColorTemp,
    kRGBWExactColors,      
    W3                     
);

struct RGBf { float r, g, b; };

typedef WS2812<LED_PIN, RGB> ControllerT;  // RGB mode must be RGB, no re-ordering allowed.
static RGBWEmulatedController<ControllerT, GRB> rgbwEmu(rgbw);  // ordering goes here.

LightStrip::LightStrip() {
  // constructor implementation
 strip = new CRGB[LED_COUNT];
}

void LightStrip::begin() {
  // Emulate RGBW output exactly like the reference example, but on this class buffer.
  FastLED.addLeds(&rgbwEmu, strip, LED_COUNT);
}

static float clamp01(float x) {
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;
  return x;
}

static RGBf kelvinToRgbNorm(uint16_t kelvin) {
  // Valid-ish range for this approximation
  float k = (float)constrain((int)kelvin, 1000, 40000) / 100.0f;

  float r, g, b;

  // Red
  if (k <= 66.0f) r = 255.0f;
  else r = 329.698727446f * std::pow(k - 60.0f, -0.1332047592f);

  // Green
  if (k <= 66.0f) g = 99.4708025861f * std::log(k) - 161.1195681661f;
  else g = 288.1221695283f * std::pow(k - 60.0f, -0.0755148492f);

  // Blue
  if (k >= 66.0f) b = 255.0f;
  else if (k <= 19.0f) b = 0.0f;
  else b = 138.5177312231f * std::log(k - 10.0f) - 305.0447927307f;

  RGBf out;
  out.r = clamp01(r / 255.0f);
  out.g = clamp01(g / 255.0f);
  out.b = clamp01(b / 255.0f);
  return out;
}

void LightStrip::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  const uint8_t rr = qadd8(r, w);
  const uint8_t gg = qadd8(g, w);
  const uint8_t bb = qadd8(b, w);

  fill_solid(strip, LED_COUNT, CRGB(rr, gg, bb));
  FastLED.show();
}

void LightStrip::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void LightStrip::setKelvin(uint16_t kelvin, uint8_t level) {
  RGBf c = kelvinToRgbNorm(kelvin);

  uint8_t r = (uint8_t)(c.r * level + 0.5f);
  uint8_t g = (uint8_t)(c.g * level + 0.5f);
  uint8_t b = (uint8_t)(c.b * level + 0.5f);

  fill_solid(strip, LED_COUNT, CRGB(r, g, b));
  FastLED.show();
}

void LightStrip::fillAndShow(CRGB color) {
  for(int i=0; i<LED_COUNT; i++) {
    strip[i] = color;
  }
  FastLED.show();
}

void LightStrip::setWhite(uint8_t w) {
  // Pure white request for RGBW emulation path.
  fill_solid(strip, LED_COUNT, CRGB(w, w, w));
  FastLED.show();
}

void LightStrip::turnOff() {
  FastLED.clear();
  FastLED.show();
}

LightStrip::~LightStrip() { delete[] strip; }