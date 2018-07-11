#ifndef Relay_h
#define Relay_h

namespace mys_toolkit {

class Relay
{
  bool state_;
  bool prevSwState_;
  int relayPin_;
  bool isRising_(bool swState);
  void updateRelayPin_();

public:
  Relay(int relayPin);
  bool update(bool currSwState);
  bool getState();
  void set(bool state);
};

} //mys_toolkit

#endif //Relay_h
