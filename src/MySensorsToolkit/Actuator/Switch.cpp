#include "Switch.h"

#include <Arduino.h>

namespace mys_toolkit {

bool Switch::update()
{
  return doUpdate_();
}

bool GPIOSwitch::doUpdate_() {
  auto state = digitalRead(pin_);
  return activeLow_ ? !state : state;
}
    
GPIOSwitch::GPIOSwitch(uint8_t pin, bool activeLow)
  : pin_(pin),
    activeLow_(activeLow)
{
  pinMode(pin_, INPUT);
}

} //mys_toolkit
