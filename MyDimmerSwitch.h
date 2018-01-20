#ifndef MyDimmerSwitch_h
#define MyDimmerSwitch_h

#include "MyMySensorsBase.h"
#include "Dimmer.h"
#include "Switch.h"

namespace mymysensors {

class MyDimmerSwitch : public MyMySensorsBase
{
  Dimmer &dim_;
  Switch &sw_;
  MyMyMessage dimmerMsg_;
  MyMyMessage lightMsg_;
  static uint8_t fromPercentage_(uint8_t percentage) {
    return uint8_t(round(255.0*percentage/100));
  }
  static uint8_t fromLevel_(uint8_t level) {
    return uint8_t(round(100.0*level/255));
  }
  void sendCurrentLevel_() {
    uint8_t percentage = fromLevel_(dim_.getLevel());
    lightMsg_.send(percentage > 0 ? 1 : 0);
    dimmerMsg_.send(percentage);
    #ifdef MY_MY_DEBUG
    Serial.print("sendCurrentLevel ");
    Serial.print(percentage);
    Serial.print(" for child id ");
    Serial.println(lightMsg_.getMyMessage().sensor);
    #endif
  }
  void firstUpdate_() override {
    sendCurrentLevel_();
  }
  void update_() override {
    if (dim_.update(sw_.update()))
      sendCurrentLevel_();
  }
  void receive_(const MyMessage &message) override {
    if (mGetCommand(message) == C_REQ) {
      sendCurrentLevel_();
    }
    else if (mGetCommand(message) == C_SET) {
      //  Retrieve the power or dim level from the incoming request message
      int requestedValue = atoi(message.data);

      if (message.type == V_DIMMER) {    
        // Clip incoming level to valid range of 0 to 100
        requestedValue = requestedValue > 100 ? 100 : requestedValue;
        requestedValue = requestedValue < 0   ? 0   : requestedValue;

        #ifdef MY_MY_DEBUG
        Serial.print("Changing dimmer [");
        Serial.print(message.sensor);
        Serial.print("] level to ");
        Serial.print(requestedValue);
        Serial.print( ", from " );
        Serial.println(fromLevel_(dim_.getLevel()));
        #endif

        dim_.request(fromPercentage_(requestedValue));
      }
      else if (message.type == V_STATUS) {
        dim_.set(requestedValue);
      }
    }
  }
public:
  MyDimmerSwitch(uint8_t sensorId, Dimmer &dim, Switch &sw)
    : MyMySensorsBase(sensorId, S_DIMMER), 
      dim_(dim),
      sw_(sw),
      dimmerMsg_(sensorId, V_DIMMER),
      lightMsg_(sensorId, V_STATUS)
  {}
};

} // mymysensors

#endif //MyDimmerSwitch_h
