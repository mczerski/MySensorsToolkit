#ifndef BounceSwitch_h
#define BounceSwitch_h

#include "Switch.h"
#include <MySensorsToolkit/Duration.h>

#include <Bounce2.h>

namespace mys_toolkit {

class BounceSwitch : public Switch {
  Bounce switch_;
  bool doUpdate_() override;

public:
  BounceSwitch(uint8_t pin, Duration interval_ms, bool activeLow = false);
};

} //mys_toolkit

#endif //BounceSwitch_h
