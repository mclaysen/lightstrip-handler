#pragma once
#include "../../../Effect.h"

class EasterEffect : public IEffect {
public:
    void begin(LightStrip& strip) override;
    void update(LightStrip& strip, uint32_t nowMs) override;
    void stop(LightStrip& strip) override;

private:
    uint32_t startMs = 0;
    uint32_t lastFrameMs = 0;
    
    static constexpr uint8_t NUM_EGGS = 3;
    struct Egg {
        uint8_t r, g, b;
    } eggs[NUM_EGGS] = {
        {255, 200, 100},  // pastel peach
        {200, 255, 150},  // pastel lime
        {255, 150, 200}   // pastel pink
    };
};