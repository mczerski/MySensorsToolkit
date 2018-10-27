#include "RelayActuator.h"

namespace mys_toolkit {

void RelayActuator::sendCurrentState_()
{
  lightMsg_.send(relay_.getState());
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("sendCurrentState ");
  Serial.print(relay_.getState());
  Serial.print(" for child id ");
  Serial.println(lightMsg_.getSensor());
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
      Serial.print("Changing relay [");
      Serial.print(message.sensor);
      Serial.print("] state to ");
      Serial.print(requestedState);
      Serial.print( ", from " );
      Serial.println(relay_.getState());
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
