#ifndef SensorValue_h
#define SensorValue_h

#include <MySensors.h>

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

  void present_() {
    ::present(sensorId_, sensorType_);
  }
  void forceResend_() {
    noUpdates_ = FORCE_UPDATE_N_READS;
  }

public:
  SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType)
    : msg_(sensorId, type), noUpdates_(0), sensorId_(sensorId), sensorType_(sensorType)
  {
    if (valuesCount_ < MAX_VALUES)
      values_[valuesCount_++] = this;
  }
  static void present() {
    for (size_t i=0; i<valuesCount_; i++)
      values_[i]->present_();
  }
  static void forceResend() {
    for (size_t i=0; i<valuesCount_; i++)
      values_[i]->forceResend_();
  }
  static void beforeUpdate() {
    success_ = true;
  }
  static void update(bool success) {
    success_ &= success;
  }
  static bool afterUpdate() {
    return success_;
  }
};

uint8_t SensorValueBase::valuesCount_ = 0;
SensorValueBase* SensorValueBase::values_[];
bool SensorValueBase::success_ = true;

template <typename ValueType>
class SensorValue : public SensorValueBase {
  ValueType lastValue_;
  ValueType treshold_;

  bool handleValue_(ValueType value) {
    bool success = true;

    if (abs(lastValue_ - value) > treshold_ || noUpdates_ == FORCE_UPDATE_N_READS) {
      #ifdef MY_MY_DEBUG
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
      }
      else {
        noUpdates_ = FORCE_UPDATE_N_READS;
        #ifdef MY_MY_DEBUG
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
    return success;
  }
  bool sendMessage_(ValueType value) {
    setMessageValue(msg_, value);
    return send(msg_, true) and wait(2000, C_SET, msg_.type);
  }
public:
  SensorValue(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType treshold = 0)
    : SensorValueBase(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold)  {}
  void update(ValueType value) {
    SensorValueBase::update(handleValue_(value));
  }
};

} //mys_toolkit

#endif //SensorValue_h
