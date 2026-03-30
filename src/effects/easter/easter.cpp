#include "easter.h"

namespace {
    constexpr uint16_t kLedCount = 60;
    constexpr uint16_t kFrameMs = 100; // 10 FPS
    constexpr uint16_t kStepMs  = 320; // slower egg movement

    uint16_t wrapIndex(int v) {
        while (v < 0) v += kLedCount;
        return (uint16_t)(v % kLedCount);
    }

    uint8_t scale8u(uint8_t value, uint8_t scale) {
        return (uint8_t)(((uint16_t)value * (uint16_t)scale) / 255);
    }

    uint8_t satAdd(uint8_t a, uint8_t b) {
        uint16_t s = (uint16_t)a + (uint16_t)b;
        return (s > 255) ? 255 : (uint8_t)s;
    }
}

void EasterEffect::begin(LightStrip& strip) {
    startMs = millis();
    lastFrameMs = 0;
}

void EasterEffect::update(LightStrip& strip, uint32_t nowMs) {
    if (nowMs - lastFrameMs < kFrameMs) return;
    lastFrameMs = nowMs;

    // Gentle global brightness
    uint8_t breath = 175 + (sin8((nowMs / 36) & 0xFF) / 5); // ~175..226
    strip.setBrightness(breath);

    // Pastel background wash (slowly drifting)
    uint8_t t = (uint8_t)((nowMs / 40) & 0xFF);
    uint8_t bgR = 22 + (sin8(t) >> 4);             // ~22..37
    uint8_t bgG = 18 + (sin8(t + 85) >> 4);        // ~18..33
    uint8_t bgB = 26 + (sin8(t + 170) >> 4);       // ~26..41

    for (uint16_t i = 0; i < kLedCount; i++) {
        strip.loadPixelColor(i, bgR, bgG, bgB, 0);
    }

    uint32_t step = nowMs / kStepMs;

    // Brighter "egg" accents on top of background
    for (uint8_t i = 0; i < NUM_EGGS; i++) {
        uint16_t offset = (kLedCount / NUM_EGGS) * i;
        uint16_t pos = (uint16_t)((step + offset) % kLedCount);

        uint8_t r = eggs[i].r, g = eggs[i].g, b = eggs[i].b;

        // center
        strip.loadPixelColor(pos,
            satAdd(bgR, scale8u(r, 150)),
            satAdd(bgG, scale8u(g, 150)),
            satAdd(bgB, scale8u(b, 150)),
            0);

        // near halo
        uint16_t p1 = wrapIndex((int)pos - 1);
        uint16_t p2 = wrapIndex((int)pos + 1);
        strip.loadPixelColor(p1,
            satAdd(bgR, scale8u(r, 90)),
            satAdd(bgG, scale8u(g, 90)),
            satAdd(bgB, scale8u(b, 90)),
            0);
        strip.loadPixelColor(p2,
            satAdd(bgR, scale8u(r, 90)),
            satAdd(bgG, scale8u(g, 90)),
            satAdd(bgB, scale8u(b, 90)),
            0);

        // far halo
        uint16_t p3 = wrapIndex((int)pos - 2);
        uint16_t p4 = wrapIndex((int)pos + 2);
        strip.loadPixelColor(p3,
            satAdd(bgR, scale8u(r, 40)),
            satAdd(bgG, scale8u(g, 40)),
            satAdd(bgB, scale8u(b, 40)),
            0);
        strip.loadPixelColor(p4,
            satAdd(bgR, scale8u(r, 40)),
            satAdd(bgG, scale8u(g, 40)),
            satAdd(bgB, scale8u(b, 40)),
            0);
    }

    strip.show();
}

void EasterEffect::stop(LightStrip& strip) {
    for (uint16_t i = 0; i < kLedCount; i++) {
        strip.loadPixelColor(i, 0, 0, 0, 0);
    }
    strip.show();
}