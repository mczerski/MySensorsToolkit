#include "BounceSwitch.h"

namespace mys_toolkit {

bool BounceSwitch::doUpdate_()
{
  switch_.update();
  return switch_.read();
}

BounceSwitch::BounceSwitch(uint8_t pin, Duration interval_ms, bool activeLow)
  : Switch(activeLow)
{
  switch_.attach(pin, activeLow ? INPUT_PULLUP : INPUT);
  switch_.interval(interval_ms.getMilis());
}
} //mys_toolkit
