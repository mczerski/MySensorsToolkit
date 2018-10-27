#include "Relay.h"

#include <Arduino.h>

namespace mys_toolkit {

bool Relay::isRising_(bool swState)
{
  return swState == true and prevSwState_ == false;
}

bool Relay::getState()
{
  return state_;
}

bool Relay::update(bool currSwState)
{
  bool retVal = false;
  if (isRising_(currSwState)) {
    state_ = not state_;
    retVal = true;
    updateState_(state_);
  }
  else {
    update_();
  }
  prevSwState_ = currSwState;
  return retVal;
}

void Relay::set(bool state)
{
  state_ = state;
  updateState_(state_);
}


void GPIORelay::updateState_(bool state)
{
  digitalWrite(relayPin_, !state);
}

GPIORelay::GPIORelay(int relayPin)
  : relayPin_(relayPin)
{
  pinMode(relayPin_, OUTPUT);
  updateState_(getState());
}

} //mys_toolkit
