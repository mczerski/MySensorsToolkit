#ifndef AnalogBounceSwitch_h
#define AnalogBounceSwitch_h

#include "Switch.h"
#include <MySensorsToolkit/Duration.h>

#include <AnalogMultiButton.h>

namespace mys_toolkit {

class AnalogBounceSwitch : public Switch {
  AnalogMultiButton button_;
  const int buttonValues_[1] = {0};
  bool doUpdate_() override;

public:
  AnalogBounceSwitch(uint8_t pin, Duration interval_ms, bool activeLow = false);
};

} //mys_toolkit

#endif //AnalogBounceSwitch_h
