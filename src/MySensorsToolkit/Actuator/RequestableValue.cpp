#include "RequestableValue.h"

namespace mys_toolkit {

void RequestableValueBase::scheduleEvent(boolean (*cb)(EventBase*), Duration delayMs)
{
  this->period = 0;
  this->repeatCount = 1;
  Duration nextTriggerTime;
  nextTriggerTime += delayMs;
  this->nextTriggerTime = nextTriggerTime.get();
  this->callback = cb;
  this->addEvent(this);
}

boolean RequestableValueBase::readValue_(EventBase* event)
{
  RequestableValueBase* value = static_cast<RequestableValueBase*>(event);
  value->updateValueCb_();
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("next measurement: ");
  Serial.println(value->interval_.getMilis());
  #endif
  value->scheduleEvent(RequestableValueBase::startMeasurement_, value->interval_);
  return true;
}

boolean RequestableValueBase::startMeasurement_(EventBase* event)
{
  RequestableValueBase* value = static_cast<RequestableValueBase*>(event);
  Duration conversionTime = value->startMeasurementCb_();
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("startMeasurement conversionTime: ");
  Serial.println(conversionTime.getMilis());
  #endif
  value->scheduleEvent(RequestableValueBase::readValue_, conversionTime);
  return true;
}

void RequestableValueBase::begin2_()
{
}

void RequestableValueBase::begin_()
{
  scheduleEvent(startMeasurement_, Duration(0));
  begin2_();
}

void RequestableValueBase::receive_(const MyMessage &message)
{
  if (message.type == sensorType_ and mGetCommand(message) == C_REQ)
    send();
}

RequestableValueBase::RequestableValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType, Duration interval)
  : ActuatorBase(sensorId, sensorType),
    interval_(interval),
    msg_(sensorId, type)
{
}

} //mys_toolkit
