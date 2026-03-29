#pragma once

#include <cstdint>

enum class Command : uint8_t {
  On,
  Off,
  ChangeBrightness,
  ChangeColor,
  ChangeKelvin,
  Unknown
};