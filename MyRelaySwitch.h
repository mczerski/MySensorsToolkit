#ifndef MyRelaySwitch_h
#define MyRelaySwitch_h

#include "MyMySensorsBase.h"
#include "Switch.h"

namespace mymysensors {

class MyRelaySwitch : public MyMySensorsBase
{
  bool state_;
  bool prevSwState_;
  int relayPin_;
  Switch &sw_;
  MyMyMessage lightMsg_;
  void sendCurrentState_() {
    lightMsg_.send(state_);
    #ifdef MY_MY_DEBUG
    Serial.print("sendCurrentState ");
    Serial.print(state_);
    Serial.print(" for child id ");
    Serial.println(lightMsg_.sensor);
    #endif
  }
  void firstUpdate_() override {
    sendCurrentState_();
  }
  void update_() override {
    bool currSwState = sw_.update();
    if (prevSwState_ != currSwState && state_ != currSwState) {
        state_ = currSwState;
        sendCurrentState_();
    }
    prevSwState_ = currSwState;
    digitalWrite(relayPin_, !state_);
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
        Serial.println(state_);
        #endif

        state_ = requestedState;
        sendCurrentState_();
      }
    }
  }
public:
  MyRelaySwitch(uint8_t sensorId, Switch &sw, int relayPin)
    : MyMySensorsBase(sensorId, S_BINARY),
      state_(false),
      prevSwState_(false),
      relayPin_(relayPin),
      sw_(sw),
      lightMsg_(sensorId, V_STATUS)
  {
    pinMode(relayPin_, OUTPUT);
  }
};

} // mymysensors

#endif //MyRelaySwitch_h
