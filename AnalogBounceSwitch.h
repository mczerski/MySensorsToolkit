#ifndef AnalogBounceSwitch_h
#define AnalogBounceSwitch_h

#include "Switch.h"

#include <AnalogMultiButton.h>

namespace mymysensors {

class AnalogBounceSwitch : public Switch {
  AnalogMultiButton button_;
  const int buttonValues_[1] = {0};
  bool doUpdate_() override {
    button_.update();
    return !button_.isPressed(0);
  }
public:
  AnalogBounceSwitch(uint8_t pin, MyDuration interval_ms, bool activeLow = false) : Switch(activeLow), button_(pin, 1, buttonValues_, interval_ms.getMilis()) {
    button_.update();
  }
};

} // mymysensors

#endif //AnalogBounceSwitch_h
