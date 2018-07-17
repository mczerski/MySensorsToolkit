#include "ActuatorBase.h"
#include <MySensorsToolkit/Message.h>
#include <MySensorsToolkit/MySensors.h>
#include <MySensorsToolkit/utils.h>

#include <SoftTimer.h>

namespace mys_toolkit {

void ActuatorBase::begin_()
{
}

void ActuatorBase::update_()
{
}

void ActuatorBase::firstUpdate_()
{
}

void ActuatorBase::receive_(const MyMessage &)
{
}

ActuatorBase::ActuatorBase(uint8_t sensorId, uint8_t sensorType)
  : sensorId_(sensorId), sensorType_(sensorType)
{
  if (sensorsCount_ < MAX_SENSORS)
    sensors_[sensorsCount_++] = this;
}

void ActuatorBase::begin()
{
  for (size_t i=0; i<sensorsCount_; i++)
    sensors_[i]->begin_();
}

void ActuatorBase::present()
{
  for (size_t i=0; i<sensorsCount_; i++)
    ::present(sensors_[i]->sensorId_, sensors_[i]->sensorType_);
}

void ActuatorBase::update()
{
  timer_.update();
  Message::update();
  for (size_t i=0; i<sensorsCount_; i++)
    sensors_[i]->update_();
  if (not loopCalled_) {
    checkTransport();
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->firstUpdate_();
    loopCalled_ = true;
  }
}

void ActuatorBase::receive(const MyMessage &message)
{
  if (message.isAck()) {
    Message::setSent(message);
    return;
  }
  for (size_t i=0; i<sensorsCount_; i++)
    if (sensors_[i]->sensorId_ == message.sensor)
      sensors_[i]->receive_(message);
}

int16_t ActuatorBase::addEvent(EventBase* evt)
{
  return timer_.addEvent(evt);
}

uint8_t ActuatorBase::sensorsCount_ = 0;
ActuatorBase * ActuatorBase::sensors_[];
bool ActuatorBase::loopCalled_ = false;
SoftTimer ActuatorBase::timer_;

} //mys_toolkit
