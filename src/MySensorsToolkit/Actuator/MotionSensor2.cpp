#include "MotionSensor2.h"

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

void MotionSensor2::begin_()
{
  pinMode(pin_, INPUT);
}

void MotionSensor2::sendMotion_(bool motion)
{
  motionMsg_.send(motion);
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("sendMotion_ ");
  MYS_TOOLKIT_SERIAL.print(motion);
  MYS_TOOLKIT_SERIAL.print(" for child id ");
  MYS_TOOLKIT_SERIAL.println(motionMsg_.getSensor());
  #endif
}

void MotionSensor2::update_()
{
  bool motion = digitalRead(pin_);
  if (state_ == NO_MOTION) {
    if (motion) {
      state_ = MOTION_HIGH;
      sendMotion_(true);
    }
  }
  else if (state_ == MOTION_HIGH) {
    if (not motion) {
      state_ = MOTION_LOW;
      motion = true;
      motionOffTimestamp_ = Duration() + Duration(1000l * triggerDelay_.get());
    }
  }
  else if (state_ == MOTION_LOW) {
    if (motion) {
      state_ = MOTION_HIGH;
    }
    else if (motionOffTimestamp_ <= Duration()) {
      state_ = NO_MOTION;
      sendMotion_(false);
    }
  }
}

MotionSensor2::MotionSensor2(uint8_t sensorId, uint8_t pin)
  : ActuatorBase(sensorId, S_MOTION),
    pin_(pin),
    state_(NO_MOTION),
    motionMsg_(sensorId, V_TRIPPED),
    triggerDelay_(sensorId, V_VAR1, S_CUSTOM, 30)
{
}

} //mys_toolkit
