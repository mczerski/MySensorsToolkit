#include "ButtonSensor.h"

namespace mys_toolkit {

bool ButtonSensor::getTripped_() const
{
  return not digitalRead(pin_);
}

bool ButtonSensor::begin_()
{
  pinMode(pin_, INPUT_PULLUP);
  lastState_ = getTripped_();
  return true;
}

unsigned long ButtonSensor::update_()
{
  bool tripped = getTripped_();
  if (not lastState_ and tripped) {
    sceneMsg_.send<uint8_t>(1);
  }
  lastState_ = tripped;
  return SLEEP_TIME;
}

ButtonSensor::ButtonSensor(uint8_t sensorId, uint8_t pin)
  : pin_(pin), sceneMsg_(sensorId, V_SCENE_ON)
{
  requestInterrupt(pin_, CHANGE);
}

} //mys_toolkit
