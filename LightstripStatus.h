#pragma once
#include <cstdint>

struct RgbwValue {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;

    String toString() const {
        return String(r) + "," + String(g) + "," + String(b) + "," + String(w);
    }
};

struct LightStripStatus {
    bool isOn;
    uint8_t brightness;
    RgbwValue rgbw;
    uint16_t temperature;
};