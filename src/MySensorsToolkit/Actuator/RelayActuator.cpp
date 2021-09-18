#include "RelayActuator.h"

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

void RelayActuator::sendCurrentState_()
{
  lightMsg_.send(relay_.getState());
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("sendCurrentState ");
  MYS_TOOLKIT_SERIAL.print(relay_.getState());
  MYS_TOOLKIT_SERIAL.print(" for child id ");
  MYS_TOOLKIT_SERIAL.println(lightMsg_.getSensor());
  #endif
}

void RelayActuator::begin_()
{
  relay_.begin();
}

void RelayActuator::firstUpdate_()
{
  sendCurrentState_();
}

void RelayActuator::update_()
{
  if (relay_.update(sw_.update()))
    sendCurrentState_();
}

void RelayActuator::receive_(const MyMessage &message)
{
  if (mGetCommand(message) == C_REQ) {
    sendCurrentState_();
  }
  else if (mGetCommand(message) == C_SET) {
    if (message.type == V_STATUS) {    
      bool requestedState = message.getBool();

      #ifdef MYS_TOOLKIT_DEBUG
      MYS_TOOLKIT_SERIAL.print("Changing relay [");
      MYS_TOOLKIT_SERIAL.print(message.sensor);
      MYS_TOOLKIT_SERIAL.print("] state to ");
      MYS_TOOLKIT_SERIAL.print(requestedState);
      MYS_TOOLKIT_SERIAL.print( ", from " );
      MYS_TOOLKIT_SERIAL.println(relay_.getState());
      #endif

      relay_.set(requestedState);
      sendCurrentState_();
    }
  }
}

RelayActuator::RelayActuator(uint8_t sensorId, Relay &relay, Switch &sw)
  : ActuatorBase(sensorId, S_BINARY),
    relay_(relay),
    sw_(sw),
    lightMsg_(sensorId, V_STATUS)
{
}

} //mys_toolkit
