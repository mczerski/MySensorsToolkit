#ifndef MotionSensor_h
#define MotionSensor_h

#include "MyMySensor.h"

namespace mymysensors {


class MotionSensor: public MyMySensor
{
  enum State {
    MOTION_HIGH,
    MOTION_LOW,
    NO_MOTION
  };
  uint8_t pin_;
  State state_;
  MyValue<uint16_t> tripped_;
  MyParameter<uint8_t> triggerDelay_;
  void begin_() override {
    pinMode(pin_, INPUT);
  }
  unsigned long update_() override {
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
public:
  MotionSensor(uint8_t sensorId, uint8_t pin)
    : pin_(pin), state_(NO_MOTION), tripped_(sensorId, V_TRIPPED, S_MOTION), triggerDelay_(sensorId, V_VAR1, S_CUSTOM, 30)
  {
    requestInterrupt(pin_, CHANGE);
  }
};

} // mymysensors

#endif //MotionSensor_h
