#pragma once

#include <stdint.h>

namespace mys_toolkit {

class Switch {
  uint8_t activeLow_;
  virtual bool doUpdate_() = 0;
public:
  Switch(bool activeLow = false);
  bool update();
};

} //mys_toolkit

