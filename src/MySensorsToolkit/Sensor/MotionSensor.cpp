#include "MotionSensor.h"

namespace mys_toolkit {

bool MotionSensor::begin_()
{
  pinMode(pin_, INPUT);
  return true;
}

unsigned long MotionSensor::update_()
{
  bool motion = digitalRead(pin_);
  unsigned long wait = -1;
  if (state_ == NO_MOTION) {
    if (motion) {
      state_ = MOTION_HIGH;
    }
  }
  else if (state_ == MOTION_HIGH) {
    if (not motion) {
      state_ = MOTION_LOW;
      motion = true;
      wait = 1000l*triggerDelay_.get() - 2300l;
    }
  }
  else if (state_ == MOTION_LOW) {
    if (motion) {
      state_ = MOTION_HIGH;
    }
    else {
      state_ = NO_MOTION;
    }
  }
  tripped_.update(motion);
  return wait;
}

MotionSensor::MotionSensor(uint8_t sensorId, uint8_t pin)
  : pin_(pin), state_(NO_MOTION), tripped_(sensorId, V_TRIPPED, S_MOTION), triggerDelay_(sensorId, V_VAR1, S_CUSTOM, 30)
{
  requestInterrupt(pin_, CHANGE);
}

} //mys_toolkit
