#pragma once
#include <cstdint>

struct LightStripStatus {
    bool isOn;
    uint8_t brightness;
    String rgbw;
    uint16_t temperature;
};