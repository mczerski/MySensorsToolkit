#pragma once

#include <MySensorsToolkit/array.h>
#include <MySensorsToolkit/Duration.h>

namespace mys_toolkit {

class DimmerDriver {
  bool inverted_;
protected:
  void driveLedPin_(uint8_t pin, uint8_t level);
public:
  DimmerDriver(bool inverted);
  virtual void setLevel(uint8_t level) = 0;
  virtual void begin() {}
};

template <size_t N>
class CwWwDimmerDriverN: public DimmerDriver {
  const array<uint8_t, N> wwPin_;
  const array<uint8_t, N> cwPin_;

  void setLevel(uint8_t level) override {
    uint8_t wwLevel = level < 128 ? 2*level : 255;
    uint8_t cwLevel = level > 128 ? 2*(level - 128) : 0;
    for (size_t n=0; n<N; n++) {
      driveLedPin_(wwPin_[n], wwLevel);
      driveLedPin_(cwPin_[n], cwLevel);
    }
  }

public:
  CwWwDimmerDriverN(const array<uint8_t, N> &wwPin, const array<uint8_t, N> &cwPin, bool inverted)
    : DimmerDriver(inverted), wwPin_(wwPin), cwPin_(cwPin)
  {}
  CwWwDimmerDriverN(uint8_t wwPin, uint8_t cwPin, bool inverted)
    : CwWwDimmerDriverN(array<uint8_t, N>{wwPin}, array<uint8_t, N>{cwPin}, inverted) {}
};

using CwWwDimmerDriver = CwWwDimmerDriverN<1>;

template <size_t N = 1>
class SimpleDimmerDriverN: public DimmerDriver {
  const array<uint8_t, N> pin_;

  void setLevel(uint8_t level) override {
    for (size_t n=0; n<N; n++) {
      driveLedPin_(pin_[n], level);
    }
  }

public:
  SimpleDimmerDriverN(const array<uint8_t, N> &pin, bool inverted)
    : DimmerDriver(inverted), pin_(pin)
  {
  }
  SimpleDimmerDriverN(uint8_t pin, bool inverted)
    : SimpleDimmerDriverN(array<uint8_t, N>{pin}, inverted) {}
};

using SimpleDimmerDriver = SimpleDimmerDriverN<1>;

class Dimmer {
  enum State {
    OFF,
    ON,
    DIMMING_UP,
    SLOW_DIMMING_UP,
    DIMMING_DOWN,
    SLOW_DIMMING_DOWN
  };
  State state_ = OFF;
  DimmerDriver & dimmerDriver_;
  uint8_t currentLevel_ = 0;
  uint8_t requestedLevel_ = 0;
  uint8_t lastLevel_ = 0;
  Duration nextChangeTime_;
  uint8_t dimmSpeed_;
  static constexpr uint8_t maxDimmSpeed_ = 20;

  Duration fadeDelay_();
  void handleDimming_();
  bool isInIdleState_();
  void triggerLevelChange_();
  bool updateLevel_();

  virtual void begin_() = 0;
  virtual bool update_(bool value) = 0;

protected:
  bool isInSlowDimming_();
  void startSlowDimming_();
  void stopSlowDimming_();

public:
  Dimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed);
  virtual ~Dimmer();
  void begin();
  bool update(bool value);
  void request(uint8_t value);
  void toggle();
  void set(bool on);
  uint8_t getLevel();
};

struct Functions {
  uint8_t slowDimming : 1;
  uint8_t fullBrightness : 1;
};

class PushPullDimmer : public Dimmer {
  Functions functions_;
  bool lastPinValue_ = false;
  Duration lastPinRiseTime_;
  bool isRising_(bool pinValue);
  bool isHeldLongEnough_(bool pinValue);
  bool isFalling(bool pinValue);
  bool isLongPress_();
  void begin_() override;
  bool update_(bool value) override ;
public:
  PushPullDimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed, Functions functions);
};

class PushDimmer : public Dimmer {
  bool lastPinValue_ = false;
  Duration lastPinRiseTime_;
  bool isRising_(bool pinValue);
  bool isDoublePress_();
  void begin_() override;
  bool update_(bool value) override ;
public:
  PushDimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed);
};

} //mys_toolkit

