#pragma once
#include <Arduino.h>
#include "LightStrip.h"

class IEffect {
public:
    virtual ~IEffect() {}
    virtual void begin(LightStrip& strip) = 0;
    virtual void update(LightStrip& strip, uint32_t nowMs) = 0; // non-blocking
    virtual void stop(LightStrip& strip) = 0;
};