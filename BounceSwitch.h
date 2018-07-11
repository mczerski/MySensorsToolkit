#ifndef BounceSwitch_h
#define BounceSwitch_h

#include "Switch.h"
#include "Duration.h"

#include <Bounce2.h>

namespace mys_toolkit {

class BounceSwitch : public Switch {
  Bounce switch_;
  bool doUpdate_() override {
    switch_.update();
    return switch_.read();
  }
public:
  BounceSwitch(uint8_t pin, Duration interval_ms, bool activeLow = false) : Switch(activeLow) {
    switch_.attach(pin, activeLow ? INPUT_PULLUP : INPUT);
    switch_.interval(interval_ms.getMilis());
  }
};

} //mys_toolkit

#endif //BounceSwitch_h
