#ifndef Dimmer_h
#define Dimmer_h

#include "array.h"
#include "Duration.h"

namespace mys_toolkit {

struct Functions {
  uint8_t slowDimming : 1;
  uint8_t fullBrightness : 1;
};

class Dimmer {
  enum State {
    OFF,
    ON,
    DIMMING_UP,
    SLOW_DIMMING_UP,
    DIMMING_DOWN,
    SLOW_DIMMING_DOWN
  };
  bool lastPinValue_;
  State state_;
  uint8_t currentLevel_;
  uint8_t requestedLevel_;
  uint8_t lastLevel_;
  Duration nextChangeTime_;
  bool inverted_;
  Duration lastPinRiseTime_;
  uint8_t dimmSpeed_;
  static constexpr uint8_t maxDimmSpeed_ = 20;
  Functions functions_;

  Duration fadeDelay_();
  virtual void setLevel_(uint8_t level) = 0;
  bool updateLevel_();
  void handleDimming_();
  bool isRising_(bool pinValue);
  bool isHeldLongEnough_(bool pinValue);
  bool isFalling(bool pinValue);
  bool isLongPress();
  bool isInSlowDimming_();
  bool isInIdleState_();
  void triggerLevelChange_();
  void startSlowDimming_();
  void stopSlowDimming_();

protected:
  void driveLedPin_(uint8_t pin, uint8_t level);
  void init_();

public:
  Dimmer(bool inverted, uint8_t dimmSpeed, Functions functions);
  virtual ~Dimmer();
  bool update(bool value);
  void request(uint8_t value);
  void set(bool on);
  uint8_t getLevel();
};

template <size_t N>
class CwWwDimmerN: public Dimmer {
  const array<uint8_t, N> wwPin_;
  const array<uint8_t, N> cwPin_;

  void setLevel_(uint8_t level) override {
    uint8_t wwLevel = level < 128 ? 2*level : 255;
    uint8_t cwLevel = level > 128 ? 2*(level - 128) : 0;
    for (size_t n=0; n<N; n++) {
      driveLedPin_(wwPin_[n], wwLevel);
      driveLedPin_(cwPin_[n], cwLevel);
    }
  }

public:
  CwWwDimmerN(const array<uint8_t, N> &wwPin, const array<uint8_t, N> &cwPin, bool inverted, uint8_t dimmSpeed, Functions functions)
    : Dimmer(inverted, dimmSpeed, functions), wwPin_(wwPin), cwPin_(cwPin) {
      init_();
  }
  CwWwDimmerN(uint8_t wwPin, uint8_t cwPin, bool inverted, uint8_t dimmSpeed, Functions functions)
    : CwWwDimmerN(array<uint8_t, N>{wwPin}, array<uint8_t, N>{cwPin}, inverted, dimmSpeed, functions) {}
};

using CwWwDimmer = CwWwDimmerN<1>;

template <size_t N = 1>
class SimpleDimmerN: public Dimmer {
  const array<uint8_t, N> pin_;

  void setLevel_(uint8_t level) override {
    for (size_t n=0; n<N; n++) {
      driveLedPin_(pin_[n], level);
    }
  }

public:
  SimpleDimmerN(const array<uint8_t, N> &pin, bool inverted, uint8_t dimmSpeed, Functions functions)
    : Dimmer(inverted, dimmSpeed, functions), pin_(pin) {
      init_();
  }
  SimpleDimmerN(uint8_t pin, bool inverted, uint8_t dimmSpeed, Functions functions)
    : SimpleDimmerN(array<uint8_t, N>{pin}, inverted, dimmSpeed, functions) {}
};

using SimpleDimmer = SimpleDimmerN<1>;

} //mys_toolkit

#endif //Dimmer_h
