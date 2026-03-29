#pragma once
#include "../../../Effect.h"

class HeartbeatEffect : public IEffect {
public:
    void begin(LightStrip& strip) override;
    void update(LightStrip& strip, uint32_t nowMs) override;
    void stop(LightStrip& strip) override;

private:
    uint32_t startMs = 0;
    uint32_t lastFrameMs = 0;
};