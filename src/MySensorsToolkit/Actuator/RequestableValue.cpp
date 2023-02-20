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
  MYS_TOOLKIT_SERIAL.print("next measurement: ");
  MYS_TOOLKIT_SERIAL.println(value->interval_.getMilis());
  #endif
  value->scheduleEvent(RequestableValueBase::startMeasurement_, value->interval_);
  return true;
}

boolean RequestableValueBase::startMeasurement_(EventBase* event)
{
  RequestableValueBase* value = static_cast<RequestableValueBase*>(event);
  Duration conversionTime = value->startMeasurementCb_();
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("startMeasurement conversionTime: ");
  MYS_TOOLKIT_SERIAL.println(conversionTime.getMilis());
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
  if (message.type == msg_.getType() and mGetCommand(message) == C_REQ)
    send();
}

RequestableValueBase::RequestableValueBase(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType, Duration interval)
  : ActuatorBase(sensorId, sensorType),
    interval_(interval),
    msg_(sensorId, type)
{
}

} //mys_toolkit
