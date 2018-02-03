#ifndef Dimmer_h
#define Dimmer_h

#include "array.h"

namespace mymysensors {

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
  MyDuration nextChangeTime_;
  bool inverted_;
  MyDuration lastPinRiseTime_;
  uint8_t dimmSpeed_;
  static constexpr uint8_t maxDimmSpeed_ = 20;
  Functions functions_;

  MyDuration fadeDelay_()
  {
    auto delayFactor = maxDimmSpeed_ - dimmSpeed_ + 1;
    if (currentLevel_ < 1*delayFactor)
      return MyDuration(25);
    else if (currentLevel_ < 2*delayFactor)
      return MyDuration(25);
    else if (currentLevel_ < 5*delayFactor)
      return MyDuration(10);
    else if (currentLevel_ < 10*delayFactor)
      return MyDuration(5);
    else
      return MyDuration(3);
  }

  virtual void setLevel_(uint8_t level) = 0;

  bool updateLevel_() {
    if (isInIdleState_())
      return false;
    MyDuration currentTime;
    if (currentTime < nextChangeTime_)
      return false;
    handleDimming_();
    setLevel_(currentLevel_);
    MyDuration fadeDelay = fadeDelay_();
    if (isInSlowDimming_())
      fadeDelay *= 3;
    nextChangeTime_ += fadeDelay;
    return isInIdleState_();
  }

  void handleDimming_() {
    if (state_ == DIMMING_DOWN) {
      if (currentLevel_ > requestedLevel_) {
        currentLevel_--;
      }
      if (currentLevel_ <= requestedLevel_) {
        currentLevel_ = requestedLevel_;
        state_ = OFF;
      }
    }
    else if (state_ == DIMMING_UP) {
      if (currentLevel_ < requestedLevel_) {
        currentLevel_++;
      }
      if (currentLevel_ >= requestedLevel_) {
        currentLevel_ = requestedLevel_;
        lastLevel_ = requestedLevel_;
        state_ = ON;
      }
    }
    else if (state_ == SLOW_DIMMING_UP) {
      if (currentLevel_ == 255) {
        currentLevel_--;
        state_ = SLOW_DIMMING_DOWN;
      }
      else {
        currentLevel_++;
      }
    }
    else if (state_ == SLOW_DIMMING_DOWN) {
      if (currentLevel_ == 1) {
        currentLevel_++;
        state_ = SLOW_DIMMING_UP;
      }
      else {
        currentLevel_--;
      }
    }
  }

  bool isRising_(bool pinValue) {
    return pinValue == true and lastPinValue_ == false;
  }

  bool isHeldLongEnough_(bool pinValue) {
    return pinValue == true and lastPinValue_ == true and lastPinRiseTime_ + MyDuration(2000) < MyDuration();
  }

  bool isFalling(bool pinValue) {
    return pinValue == false and lastPinValue_ == true;
  }

  bool isLongPress() {
    return lastPinRiseTime_ + MyDuration(500) < MyDuration();
  }

  bool isInSlowDimming_() {
    return state_ == SLOW_DIMMING_DOWN or state_ == SLOW_DIMMING_UP;
  }

  bool isInIdleState_() {
    return state_ == OFF or state_ == ON;
  }

  void triggerLevelChange_() {
    nextChangeTime_ = MyDuration();
  }

  void startSlowDimming_() {
    triggerLevelChange_();
    if (currentLevel_ > 255/2) {
      state_ = SLOW_DIMMING_DOWN;
    }
    else {
      state_ = SLOW_DIMMING_UP;
    }
  }

  void stopSlowDimming_() {
      requestedLevel_ = currentLevel_;
      lastLevel_ = currentLevel_;
      state_ = ON;
  }

protected:
  void driveLedPin_(uint8_t pin, uint8_t level) {
    analogWrite(pin, inverted_ ? 255 - level : level);
  }

  void init_() {
    setLevel_(currentLevel_);
  }

public:
  Dimmer(bool inverted, uint8_t dimmSpeed, Functions functions)
    : lastPinValue_(false), state_(OFF), currentLevel_(0),
      requestedLevel_(0), lastLevel_(0), inverted_(inverted),
      dimmSpeed_(min(maxDimmSpeed_, dimmSpeed)),
      functions_(functions) {}

  virtual ~Dimmer() {}

  bool update(bool value) {
    if (isRising_(value)) {
      if (not functions_.slowDimming and not functions_.fullBrightness)
        set(state_ == OFF);
      else
        lastPinRiseTime_ = MyDuration();
    }
    else if (isHeldLongEnough_(value)) {
      if (not isInSlowDimming_() and functions_.slowDimming) {
        triggerLevelChange_();
        startSlowDimming_();
      }
    }
    else if (isFalling(value)) {
      if (isInSlowDimming_() and functions_.slowDimming) {
        stopSlowDimming_();
        lastPinValue_ = value;
        return true;
      }
      else if (isLongPress() and functions_.fullBrightness) {
        request(255);
      }
      else if (functions_.slowDimming or functions_.fullBrightness) {
        set(state_ == OFF or state_ == DIMMING_DOWN);
      }
    }
    lastPinValue_ = value;
    return updateLevel_();
  }

  void request(uint8_t value) {
    if (isInSlowDimming_() or value == currentLevel_) {
      return;
    }
    triggerLevelChange_();
    if (value < currentLevel_) {
      state_ = DIMMING_DOWN;
    }
    else {
      state_ = DIMMING_UP;
    }
    requestedLevel_ = value;
  }

  void set(bool on) {
    if (on) {
      request(lastLevel_ ? lastLevel_ : 255);
    }
    else {
      request(0);
    }
  }

  uint8_t getLevel() {
    return currentLevel_;
  }

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

} // mymysensors

#endif //Dimmer_h
