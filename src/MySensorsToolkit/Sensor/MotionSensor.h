#pragma once

#include "SensorBase.h"
#include "SensorValue.h"
#include "Parameter.h"

namespace mys_toolkit {

class MotionSensor: public SensorBase
{
  enum State {
    MOTION_HIGH,
    MOTION_LOW,
    NO_MOTION
  };
  uint8_t pin_;
  State state_;
  SensorValue<uint16_t> tripped_;
  Parameter<uint8_t> triggerDelay_;
  bool begin_() override;
  unsigned long update_() override;

public:
  MotionSensor(uint8_t sensorId, uint8_t pin);
};

} //mys_toolkit

