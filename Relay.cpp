#include "Relay.h"

#include <Arduino.h>

namespace mys_toolkit {

bool Relay::isRising_(bool swState)
{
  return swState == true and prevSwState_ == false;
}

void Relay::updateRelayPin_()
{
  digitalWrite(relayPin_, !state_);
}

Relay::Relay(int relayPin)
  : state_(false),
    prevSwState_(false),
    relayPin_(relayPin)
{
  pinMode(relayPin_, OUTPUT);
}

bool Relay::update(bool currSwState)
{
  bool retVal = false;
  if (isRising_(currSwState)) {
    state_ = not state_;
    retVal = true;
  }
  prevSwState_ = currSwState;
  updateRelayPin_();
  return retVal;
}

bool Relay::getState()
{
  return state_;
}

void Relay::set(bool state)
{
  state_ = state;
  updateRelayPin_();
}

} //mys_toolkit
