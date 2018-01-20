#ifndef MyRequestingValue_h
#define MyRequestingValue_h

#include "MyMySensorsBase.h"

namespace mymysensors {

template <typename ValueType>
class MyRequestingValue : public EventBase, public MyMySensorsBase
{
  MyMyMessage msg_;
  uint8_t childId_;
  uint8_t sensorType_;
  ValueType value_;
  MyDuration interval_;
  void scheduleEvent(boolean (*cb)(EventBase*), MyDuration delayMs)
  {
    this->period = 0;
    this->repeatCount = 1;
    MyDuration nextTriggerTime;
    nextTriggerTime += delayMs;
    this->nextTriggerTime = nextTriggerTime.get();
    this->callback = cb;
    this->addEvent(this);
  }
  static boolean readValue_(EventBase* event)
  {
    MyRequestingValue* myRequestingValue = static_cast<MyRequestingValue*>(event);
    myRequestingValue->value_ = myRequestingValue->readValueCb_();
    #ifdef MY_MY_DEBUG
    Serial.print("readValue: ");
    Serial.print(myRequestingValue->value_);
    Serial.print(" next measurement: ");
    Serial.println(myRequestingValue->interval_.getMilis());
    #endif
    myRequestingValue->scheduleEvent(MyRequestingValue::startMeasurement_, myRequestingValue->interval_);
    return true;
  }
  static boolean startMeasurement_(EventBase* event)
  {
    MyRequestingValue* myRequestingValue = static_cast<MyRequestingValue*>(event);
    MyDuration conversionTime = myRequestingValue->startMeasurementCb_();
    #ifdef MY_MY_DEBUG
    Serial.print("startMeasurement conversionTime: ");
    Serial.println(conversionTime.getMilis());
    #endif
    myRequestingValue->scheduleEvent(MyRequestingValue::readValue_, conversionTime);
    return true;
  }
  virtual void begin2_() {}
  void begin_() override {
    scheduleEvent(startMeasurement_, MyDuration(0));
    begin2_();
  }
  void receive_(const MyMessage &message) override {
    if (message.type == sensorType_ and mGetCommand(message) == C_REQ)
      msg_.send(value_);
  }
  virtual ValueType readValueCb_() = 0;
  virtual MyDuration startMeasurementCb_() = 0;

public:
  MyRequestingValue(uint8_t sensorId, uint8_t type, uint8_t sensorType, MyDuration interval)
    : MyMySensorsBase(sensorId, sensorType),
      msg_(sensorId, type),
      interval_(interval)
  {}
};

} // mymysensors

#endif //MyRequestingValue_h
