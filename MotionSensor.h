#ifndef MotionSensor_h
#define MotionSensor_h

#include "MyMySensor.h"

namespace mymysensors {


class MotionSensor: public MyMySensor
{
  uint8_t pin_;
  MyValue<uint16_t> tripped_;
  void begin_() override {
    pinMode(pin_, INPUT_PULLUP);
  }
  void update_() override {
    tripped_.update(digitalRead(pin_));
  }
public:
  MotionSensor(uint8_t sensorId, uint8_t pin)
    : pin_(pin), tripped_(sensorId, V_TRIPPED, S_MOTION)
  {
    requestInterrupt(pin_, CHANGE);
  }
};

} // mymysensors

#endif //MotionSensor_h
