#pragma once

namespace mys_toolkit {

class Relay
{
  bool state_ = false;
  bool prevSwState_ = false;
  bool isRising_(bool swState);
  virtual void updateState_(bool state) = 0;
  virtual void update_() {}
public:
  virtual void begin() {};
  bool update(bool currSwState);
  bool getState();
  void set(bool state);
};

class GPIORelay : public Relay
{
  int relayPin_;
  void updateState_(bool state) override;

public:
  GPIORelay(int relayPin);
};

} //mys_toolkit

