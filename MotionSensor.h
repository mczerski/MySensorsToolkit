#ifndef MotionSensor_h
#define MotionSensor_h

#include "SensorBase.h"

namespace mys_toolkit {

class MotionSensor: public SensorBase
{
  enum State {
    MOTION_HIGH,
    MOTION_LOW,
    NO_MOTION
  };
  static const unsigned long TRIGGER_DELAY = 30000 - 2000;
  uint8_t pin_;
  State state_;
  SensorValue<uint16_t> tripped_;
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
        wait = TRIGGER_DELAY;
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
    : pin_(pin), state_(NO_MOTION), tripped_(sensorId, V_TRIPPED, S_MOTION)
  {
    requestInterrupt(pin_, CHANGE);
  }
};

} //

#endif //MotionSensor_h
