#pragma once

#include <MySensorsToolkit/utils.h>
#include <MySensorsToolkit/Message.h>

#include <core/MyMessage.h>

namespace mys_toolkit {

class SensorValueBase {
protected:
  static constexpr uint8_t FORCE_UPDATE_N_READS = 10;
  Message msg_;
  uint8_t noUpdates_;
  uint8_t sensorType_;
  static uint8_t valuesCount_;
  static constexpr uint8_t MAX_VALUES = 10;
  static SensorValueBase* values_[MAX_VALUES];

  void present_();
  void forceResend_();

public:
  SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType);
  static void present();
  static void forceResend();
};

template <typename ValueType>
class SensorValue : public SensorValueBase {
  ValueType lastValue_;
  ValueType treshold_;

public:
  SensorValue(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType treshold = 0)
    : SensorValueBase(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold)  {}
  void update(ValueType value) {
    if (abs(lastValue_ - value) > treshold_) {
      #ifdef MYS_TOOLKIT_DEBUG
      Serial.print("t=");
      Serial.print(msg_.getType());
      Serial.print(",c=");
      Serial.print(msg_.getSensor());
      Serial.print(". last=");
      Serial.print(lastValue_);
      Serial.print(", curent=");
      Serial.print(value);
      Serial.print(", noUpdates=");
      Serial.println(noUpdates_);
      #endif
      lastValue_ = value;

      msg_.send(value);
      noUpdates_ = 0;
    } else {
      noUpdates_++;
    }
  }
};

} //mys_toolkit

