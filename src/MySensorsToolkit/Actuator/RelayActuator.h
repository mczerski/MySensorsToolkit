#ifndef RelayActuator_h
#define RelayActuator_h

#include "ActuatorBase.h"
#include "Relay.h"
#include "Switch.h"
#include "Message.h"

namespace mys_toolkit {

class RelayActuator : public ActuatorBase
{
  Relay &relay_;
  Switch &sw_;
  Message lightMsg_;
  void sendCurrentState_();
  void firstUpdate_() override;
  void update_() override;
  void receive_(const MyMessage &message) override;

public:
  RelayActuator(uint8_t sensorId, Relay &relay, Switch &sw);
};

} //mys_toolkit

#endif //RelayActuator_h
