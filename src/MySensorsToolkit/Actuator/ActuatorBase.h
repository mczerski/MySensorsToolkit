#ifndef ActuatorBase_h
#define ActuatorBase_h

#include <core/MyMessage.h>
#include <SoftTimer.h>

namespace mys_toolkit {

class ActuatorBase
{
  virtual void begin_();
  virtual void update_();
  virtual void firstUpdate_();
  virtual void receive_(const MyMessage &);
  uint8_t sensorId_;
  uint8_t sensorType_;
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static ActuatorBase* sensors_[MAX_SENSORS];
  static bool loopCalled_;
  static SoftTimer timer_;

public:
  ActuatorBase(uint8_t sensorId, uint8_t sensorType);
  static void begin();
  static void present();
  static void update();
  static void receive(const MyMessage &message);
  static int16_t addEvent(EventBase* evt);
};

} //mys_toolkit

#endif //ActuatorBase_h
