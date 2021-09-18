#pragma once

#include <MySensorsToolkit/utils.h>
#include <MySensorsToolkit/Message.h>

#include <core/MyMessage.h>

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

class SensorValueBase {
protected:
  Message msg_;
  bool forceResend_ = false;
  mysensors_sensor_t sensorType_;
  static uint8_t valuesCount_;
  static constexpr uint8_t MAX_VALUES = 10;
  static SensorValueBase* values_[MAX_VALUES];

  void present_();

public:
  SensorValueBase(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType);
  static void present();
  void forceResend();
  static void forceResendAll();
};

template <typename ValueType>
class SensorValue : public SensorValueBase {
  ValueType lastValue_;
  ValueType treshold_;

public:
  SensorValue(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType, ValueType treshold = 0)
    : SensorValueBase(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold)  {}
  void update(ValueType value) {
    if (abs(lastValue_ - value) > treshold_ or forceResend_) {
      #ifdef MYS_TOOLKIT_DEBUG
      MYS_TOOLKIT_SERIAL.print("t=");
      MYS_TOOLKIT_SERIAL.print(msg_.getType());
      MYS_TOOLKIT_SERIAL.print(",c=");
      MYS_TOOLKIT_SERIAL.print(msg_.getSensor());
      MYS_TOOLKIT_SERIAL.print(". last=");
      MYS_TOOLKIT_SERIAL.print(lastValue_);
      MYS_TOOLKIT_SERIAL.print(", curent=");
      MYS_TOOLKIT_SERIAL.print(value);
      #endif
      lastValue_ = value;

      msg_.send(value);
      forceResend_ = false;
    }
  }
};

} //mys_toolkit

