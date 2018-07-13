#ifndef MotionSensor_h
#define MotionSensor_h

#include "SensorBase.h"
#include "SensorValue.h"

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
  bool begin_() override;
  unsigned long update_() override;

public:
  MotionSensor(uint8_t sensorId, uint8_t pin);
};

} //mys_toolkit

#endif //MotionSensor_h
