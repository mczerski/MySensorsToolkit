#pragma once

#include "SensorBase.h"
#include "SensorValue.h"

namespace mys_toolkit {

class ButtonSensor: public SensorBase
{
  uint8_t pin_;
  bool lastState_ = false;
  Message sceneMsg_;
  bool getTripped_() const;
  bool begin_() override;
  unsigned long update_() override;

public:
  ButtonSensor(uint8_t sensorId, uint8_t pin);
};

} //mys_toolkit
