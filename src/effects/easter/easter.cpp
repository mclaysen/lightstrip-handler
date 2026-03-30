#include "easter.h"

void EasterEffect::begin(LightStrip& strip) {
    startMs = millis();
    lastFrameMs = 0;
}

void EasterEffect::update(LightStrip& strip, uint32_t nowMs) {
    if (nowMs - lastFrameMs < 250) return; // slower updates
    lastFrameMs = nowMs;

    uint8_t colorIndex = ((nowMs - startMs) / 2000) % NUM_EGGS; // change every 2 sec

    CRGB color = CRGB(
        eggs[colorIndex].r,
        eggs[colorIndex].g,
        eggs[colorIndex].b
    );

    strip.setColor(color.r, color.g, color.b, 0);
}

void EasterEffect::stop(LightStrip& strip) {
}