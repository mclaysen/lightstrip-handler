#include "Heartbeat.h"

static uint8_t heartbeatBrightness(uint16_t phaseMs) {
    if (phaseMs < 100)  return map(phaseMs,   0, 100,   0, 255);
    if (phaseMs < 200)  return map(phaseMs, 100, 200, 255,   0);
    if (phaseMs < 300)  return map(phaseMs, 200, 300,   0, 255);
    if (phaseMs < 400)  return map(phaseMs, 300, 400, 255,   0);
    return 0;
}

void HeartbeatEffect::begin(LightStrip& strip) {
    startMs = millis();
    lastFrameMs = 0;
}

void HeartbeatEffect::update(LightStrip& strip, uint32_t nowMs) {
    if (nowMs - lastFrameMs < 20) return; // ~50 FPS, non-blocking
    lastFrameMs = nowMs;

    uint16_t phase = (nowMs - startMs) % 1000;
    uint8_t b = heartbeatBrightness(phase);
    strip.setBrightnessAndShow(b);
}

void HeartbeatEffect::stop(LightStrip& strip) {
    //strip.turnOff();
}