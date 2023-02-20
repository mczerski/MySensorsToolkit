#pragma once

#include "ActuatorBase.h"
#include <MySensorsToolkit/Message.h>
#include <MySensorsToolkit/Duration.h>

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

class RequestableValueBase : public EventBase, public ActuatorBase
{
  Duration interval_;
  void scheduleEvent(boolean (*cb)(EventBase*), Duration delayMs);
  static boolean readValue_(EventBase* event);
  static boolean startMeasurement_(EventBase* event);
  void begin_() override;
  void receive_(const MyMessage &message) override;
  virtual void begin2_();
  virtual void updateValueCb_() = 0;
  virtual void send() = 0;
  virtual Duration startMeasurementCb_() = 0;

protected:
  Message msg_;

public:
  RequestableValueBase(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType, Duration interval);
};

template <typename ValueType>
class RequestableValue : public RequestableValueBase
{
  ValueType value_;
  void updateValueCb_() override {
    value_ = readValueCb_();
    #ifdef MYS_TOOLKIT_DEBUG
    MYS_TOOLKIT_SERIAL.print("readValue: ");
    MYS_TOOLKIT_SERIAL.println(value_);
    #endif
  }
  void send() {
    msg_.send(value_);
  }
  virtual ValueType readValueCb_() = 0;
public:
  RequestableValue(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType, Duration interval)
    : RequestableValueBase(sensorId, type, sensorType, interval)
{
}
};

} //mys_toolkit

