#pragma once

#include <cstdint>

enum class Command : uint8_t {
  On,
  Off,
  ChangeBrightness,
  ChangeColor,
  ChangeTemperature,
  SetEffect,
  Unknown
};