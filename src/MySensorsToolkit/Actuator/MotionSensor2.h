#pragma once

#include "ActuatorBase.h"
#include <MySensorsToolkit/Message.h>
#include <MySensorsToolkit/Duration.h>
#include <MySensorsToolkit/Parameter.h>

namespace mys_toolkit {

class MotionSensor2 : public ActuatorBase
{
  uint8_t pin_;
  enum State {
    MOTION_HIGH,
    MOTION_LOW,
    NO_MOTION
  };
  State state_;
  Message motionMsg_;
  Duration motionOffTimestamp_;
  Parameter<uint8_t> triggerDelay_;
  void sendMotion_(bool motion);
  void begin_() override;
  void update_() override;

public:
  MotionSensor2(uint8_t sensorId, uint8_t pin);

};

} //mys_toolkit


