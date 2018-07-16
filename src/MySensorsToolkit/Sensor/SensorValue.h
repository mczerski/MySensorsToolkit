#pragma once

#include <MySensorsToolkit/utils.h>

#include <core/MyMessage.h>

namespace mys_toolkit {

class SensorValueBase {
protected:
  static constexpr uint8_t FORCE_UPDATE_N_READS = 10;
  MyMessage msg_;
  uint8_t noUpdates_;
  uint8_t sensorId_;
  uint8_t sensorType_;
  static uint8_t valuesCount_;
  static constexpr uint8_t MAX_VALUES = 10;
  static SensorValueBase* values_[MAX_VALUES];
  static bool success_;
  static bool somethingSent_;

  void present_();
  void forceResend_();

public:
  SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType);
  static void present();
  static void forceResend();
  static void beforeUpdate();
  static void update(bool success, bool somethingSent);
  static bool wasSuccess();
  static bool wasSomethingSent();
};

template <typename ValueType>
class SensorValue : public SensorValueBase {
  ValueType lastValue_;
  ValueType treshold_;

  bool sendMessage_(ValueType value) {
    setMessageValue(msg_, value);
    return sendAndWait(msg_, 2000);
  }

public:
  SensorValue(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType treshold = 0)
    : SensorValueBase(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold)  {}
  void update(ValueType value) {
    bool success = true;
    bool somethingSent = false;

    if (abs(lastValue_ - value) > treshold_ || noUpdates_ == FORCE_UPDATE_N_READS) {
      #ifdef MYS_TOOLKIT_DEBUG
      Serial.print("t=");
      Serial.print(msg_.type);
      Serial.print(",c=");
      Serial.print(msg_.sensor);
      Serial.print(". last=");
      Serial.print(lastValue_);
      Serial.print(", curent=");
      Serial.print(value);
      Serial.print(", noUpdates=");
      Serial.println(noUpdates_);
      #endif
      lastValue_ = value;

      success = sendMessage_(value);
      if (success) {
        noUpdates_ = 0;
        somethingSent = true;
      }
      else {
        noUpdates_ = FORCE_UPDATE_N_READS;
        #ifdef MYS_TOOLKIT_DEBUG
        Serial.print("t=");
        Serial.print(msg_.type);
        Serial.print(",c=");
        Serial.print(msg_.sensor);
        Serial.println(". Send failed");
        #endif
      }
    } else {
      noUpdates_++;
    }
    SensorValueBase::update(success, somethingSent);
  }
};

} //mys_toolkit

