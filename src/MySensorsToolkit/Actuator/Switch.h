#pragma once

#include <stdint.h>

namespace mys_toolkit {

class Switch {
  virtual bool doUpdate_() = 0;
public:
  bool update();
};

class GPIOSwitch: public Switch {
  uint8_t pin_;
  bool activeLow_;
  bool doUpdate_() override;
public:
  GPIOSwitch(uint8_t pin, bool activeLow = false);
};


} //mys_toolkit

