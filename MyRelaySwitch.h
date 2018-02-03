#ifndef MyRelaySwitch_h
#define MyRelaySwitch_h

#include "MyMySensorsBase.h"
#include "Relay.h"
#include "Switch.h"

namespace mymysensors {

class MyRelaySwitch : public MyMySensorsBase
{
  Relay &relay_;
  Switch &sw_;
  MyMyMessage lightMsg_;
  void sendCurrentState_() {
    lightMsg_.send(relay_.getState());
    #ifdef MY_MY_DEBUG
    Serial.print("sendCurrentState ");
    Serial.print(relay_.getState());
    Serial.print(" for child id ");
    Serial.println(lightMsg_.getMyMessage().sensor);
    #endif
  }
  void firstUpdate_() override {
    sendCurrentState_();
  }
  void update_() override {
    if (relay_.update(sw_.update()))
      sendCurrentState_();
  }
  void receive_(const MyMessage &message) override {
    if (mGetCommand(message) == C_REQ) {
      sendCurrentState_();
    }
    else if (mGetCommand(message) == C_SET) {
      if (message.type == V_STATUS) {    
        bool requestedState = message.getBool();

        #ifdef MY_MY_DEBUG
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
public:
  MyRelaySwitch(uint8_t sensorId, Relay &relay, Switch &sw)
    : MyMySensorsBase(sensorId, S_BINARY),
      relay_(relay),
      sw_(sw),
      lightMsg_(sensorId, V_STATUS)
  {
  }
};

} // mymysensors

#endif //MyRelaySwitch_h
