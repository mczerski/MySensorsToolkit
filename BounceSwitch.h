#ifndef BounceSwitch_h
#define BounceSwitch_h

#include "Switch.h"

#include <Bounce2.h>

namespace mymysensors {

class BounceSwitch : public Switch {
  Bounce switch_;
  bool doUpdate_() override {
    switch_.update();
    return switch_.read();
  }
public:
  BounceSwitch(uint8_t pin, MyDuration interval_ms, bool activeLow = false) : Switch(activeLow) {
    switch_.attach(pin, activeLow ? INPUT_PULLUP : INPUT);
    switch_.interval(interval_ms.getMilis());
  }
};

} // mymysensors

#endif //BounceSwitch_h
