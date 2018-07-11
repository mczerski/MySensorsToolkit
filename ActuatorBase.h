#ifndef ActuatorBase_h
#define ActuatorBase_h

#include "Message.h"
#include "Duration.h"
#include "utils.h"

#include <SoftTimer.h>

namespace mys_toolkit {

class ActuatorBase
{
  virtual void begin_() {}
  virtual void update_() {}
  virtual void firstUpdate_() {}
  virtual void receive_(const MyMessage &) {}
  uint8_t sensorId_;
  uint8_t sensorType_;
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static ActuatorBase* sensors_[MAX_SENSORS];
  static bool loopCalled_;
  static SoftTimer timer_;

public:
  ActuatorBase(uint8_t sensorId, uint8_t sensorType)
    : sensorId_(sensorId), sensorType_(sensorType)
  {
    if (sensorsCount_ < MAX_SENSORS)
      sensors_[sensorsCount_++] = this;
  }
  static void begin() {
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->begin_();
  }
  static void present() {
    for (size_t i=0; i<sensorsCount_; i++)
      ::present(sensors_[i]->sensorId_, sensors_[i]->sensorType_);
  }
  static void update() {
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
  static void receive(const MyMessage &message) {
    if (message.isAck()) {
      Message::setSent(message);
      return;
    }
    for (size_t i=0; i<sensorsCount_; i++)
      if (sensors_[i]->sensorId_ == message.sensor)
        sensors_[i]->receive_(message);
  }
  static int16_t addEvent(EventBase* evt) {
    return timer_.addEvent(evt);
  }
};

uint8_t ActuatorBase::sensorsCount_ = 0;
ActuatorBase * ActuatorBase::sensors_[];
bool ActuatorBase::loopCalled_ = false;
SoftTimer ActuatorBase::timer_;

} //mys_toolkit

#endif //ActuatorBase_h
