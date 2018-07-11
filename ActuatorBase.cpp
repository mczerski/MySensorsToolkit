#include "ActuatorBase.h"
#include "Message.h"
#include "utils.h"

#include <SoftTimer.h>

extern bool present(const uint8_t sensorId, const uint8_t sensorType,
                    const char *description="", const bool ack=false);

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

static void ActuatorBase::begin()
{
  for (size_t i=0; i<sensorsCount_; i++)
    sensors_[i]->begin_();
}

static void ActuatorBase::present()
{
  for (size_t i=0; i<sensorsCount_; i++)
    ::present(sensors_[i]->sensorId_, sensors_[i]->sensorType_);
}

static void ActuatorBase::update()
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

static void ActuatorBase::receive(const MyMessage &message)
{
  if (message.isAck()) {
    Message::setSent(message);
    return;
  }
  for (size_t i=0; i<sensorsCount_; i++)
    if (sensors_[i]->sensorId_ == message.sensor)
      sensors_[i]->receive_(message);
}

static int16_t ActuatorBase::addEvent(EventBase* evt)
{
  return timer_.addEvent(evt);
}

uint8_t ActuatorBase::sensorsCount_ = 0;
ActuatorBase * ActuatorBase::sensors_[];
bool ActuatorBase::loopCalled_ = false;
SoftTimer ActuatorBase::timer_;

} //mys_toolkit
