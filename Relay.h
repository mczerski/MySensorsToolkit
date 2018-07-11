#ifndef Relay_h
#define Relay_h

#include "SensorBasesBase.h"

namespace mys_toolkit {

class Relay
{
  bool state_;
  bool prevSwState_;
  int relayPin_;
  bool isRising_(bool swState) {
    return swState == true and prevSwState_ == false;
  }
  void updateRelayPin_() {
    digitalWrite(relayPin_, !state_);
  }
public:
  Relay(int relayPin)
    : state_(false),
      prevSwState_(false),
      relayPin_(relayPin)
  {
    pinMode(relayPin_, OUTPUT);
  }
  bool update(bool currSwState) {
    bool retVal = false;
    if (isRising_(currSwState)) {
      state_ = not state_;
      retVal = true;
    }
    prevSwState_ = currSwState;
    updateRelayPin_();
    return retVal;
  }
  bool getState() {
    return state_;
  }
  void set(bool state) {
    state_ = state;
    updateRelayPin_();
  }
};

} //mys_toolkit

#endif //Relay_h
