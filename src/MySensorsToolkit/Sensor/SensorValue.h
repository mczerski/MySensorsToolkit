#pragma once

#include <MySensorsToolkit/utils.h>
#include <MySensorsToolkit/Message.h>

#include <core/MyMessage.h>

namespace mys_toolkit {

class SensorValueBase {
protected:
  Message msg_;
  bool forceResend_ = false;
  uint8_t sensorType_;
  static uint8_t valuesCount_;
  static constexpr uint8_t MAX_VALUES = 10;
  static SensorValueBase* values_[MAX_VALUES];

  void present_();

public:
  SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType);
  static void present();
  void forceResend();
  static void forceResendAll();
};

template <typename ValueType>
class SensorValue : public SensorValueBase {
  ValueType lastValue_;
  ValueType treshold_;

public:
  SensorValue(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType treshold = 0)
    : SensorValueBase(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold)  {}
  void update(ValueType value) {
    if (abs(lastValue_ - value) > treshold_ or forceResend_) {
      #ifdef MYS_TOOLKIT_DEBUG
      Serial.print("t=");
      Serial.print(msg_.getType());
      Serial.print(",c=");
      Serial.print(msg_.getSensor());
      Serial.print(". last=");
      Serial.print(lastValue_);
      Serial.print(", curent=");
      Serial.print(value);
      #endif
      lastValue_ = value;

      msg_.send(value);
      forceResend_ = false;
    }
  }
};

} //mys_toolkit

