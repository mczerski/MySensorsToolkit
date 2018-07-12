#include "AnalogBounceSwitch.h"

namespace mys_toolkit {

bool AnalogBounceSwitch::doUpdate_()
{
  button_.update();
  return !button_.isPressed(0);
}

AnalogBounceSwitch::AnalogBounceSwitch(uint8_t pin, Duration interval_ms,
                                       bool activeLow = false)
  : Switch(activeLow), button_(pin, 1, {0}, interval_ms.getMilis())
{
  button_.update();
}

} //mys_toolkit
