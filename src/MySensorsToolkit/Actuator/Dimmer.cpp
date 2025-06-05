#include "Dimmer.h"

namespace mys_toolkit {

DimmerDriver::DimmerDriver(bool inverted): inverted_(inverted) {}

void DimmerDriver::driveLedPin_(uint8_t pin, uint8_t level) {
  analogWrite(pin, inverted_ ? 255 - level : level);
}

Duration Dimmer::fadeDelay_()
{
  auto delayFactor = maxDimmSpeed_ - dimmSpeed_ + 1;
  if (currentLevel_ < 1*delayFactor)
    return Duration(25);
  else if (currentLevel_ < 2*delayFactor)
    return Duration(25);
  else if (currentLevel_ < 5*delayFactor)
    return Duration(10);
  else if (currentLevel_ < 10*delayFactor)
    return Duration(5);
  else
    return Duration(3);
}

bool Dimmer::updateLevel_()
{
  if (isInIdleState_())
    return false;
  Duration currentTime;
  if (currentTime < nextChangeTime_)
    return false;
  handleDimming_();
  dimmerDriver_.setLevel(currentLevel_);
  Duration fadeDelay = fadeDelay_();
  if (isInSlowDimming_())
    fadeDelay *= 3;
  nextChangeTime_ += fadeDelay;
  return isInIdleState_();
}

void Dimmer::handleDimming_()
{
  if (state_ == DIMMING_DOWN) {
    if (currentLevel_ > requestedLevel_) {
      currentLevel_--;
    }
    if (currentLevel_ <= requestedLevel_) {
      currentLevel_ = requestedLevel_;
      if (currentLevel_) {
        lastLevel_ = requestedLevel_;
        state_ = ON;
      }
      else {
        state_ = OFF;
      }
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

bool Dimmer::isInSlowDimming_()
{
  return state_ == SLOW_DIMMING_DOWN or state_ == SLOW_DIMMING_UP;
}

bool Dimmer::isInIdleState_()
{
  return state_ == OFF or state_ == ON;
}

void Dimmer::triggerLevelChange_()
{
  nextChangeTime_ = Duration();
}

void Dimmer::startSlowDimming_()
{
  triggerLevelChange_();
  if (currentLevel_ > 255/2) {
    state_ = SLOW_DIMMING_DOWN;
  }
  else {
    state_ = SLOW_DIMMING_UP;
  }
}

void Dimmer::stopSlowDimming_()
{
    requestedLevel_ = currentLevel_;
    lastLevel_ = currentLevel_;
    state_ = ON;
}

Dimmer::Dimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed)
  : dimmerDriver_(dimmerDriver), dimmSpeed_(min(maxDimmSpeed_, dimmSpeed))
{
  dimmerDriver_.setLevel(currentLevel_);
}

Dimmer::~Dimmer()
{
}

void Dimmer::begin() {
  dimmerDriver_.begin();
}

bool Dimmer::update(bool value) {
  update_(value);
  return updateLevel_();
}

void Dimmer::request(uint8_t value)
{
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

void Dimmer::toggle() {
  set(state_ == OFF or state_ == DIMMING_DOWN);
}

void Dimmer::set(bool on)
{
  if (on) {
    request(lastLevel_ ? lastLevel_ : 255);
  }
  else {
    request(0);
  }
}

uint8_t Dimmer::getLevel()
{
  return currentLevel_;
}

PushPullDimmer::PushPullDimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed, Functions functions)
  : Dimmer(dimmerDriver, dimmSpeed), functions_(functions) {}

void PushPullDimmer::begin_() {
}

bool PushPullDimmer::update_(bool value)
{
  if (isRising_(value)) {
    if (not functions_.slowDimming and not functions_.fullBrightness)
      toggle();
    else
      lastPinRiseTime_ = Duration();
  }
  else if (isHeldLongEnough_(value)) {
    if (not isInSlowDimming_() and functions_.slowDimming) {
      startSlowDimming_();
    }
  }
  else if (isFalling(value)) {
    if (isInSlowDimming_() and functions_.slowDimming) {
      stopSlowDimming_();
      lastPinValue_ = value;
      return true;
    }
    else if (isLongPress_() and functions_.fullBrightness) {
      request(255);
    }
    else if (functions_.slowDimming or functions_.fullBrightness) {
      toggle();
    }
  }
  lastPinValue_ = value;
}

bool PushPullDimmer::isRising_(bool pinValue)
{
  return pinValue == true and lastPinValue_ == false;
}

bool PushPullDimmer::isHeldLongEnough_(bool pinValue)
{
  return pinValue == true and lastPinValue_ == true and lastPinRiseTime_ + Duration(2000) < Duration();
}

bool PushPullDimmer::isFalling(bool pinValue)
{
  return pinValue == false and lastPinValue_ == true;
}

bool PushPullDimmer::isLongPress_()
{
  return lastPinRiseTime_ + Duration(500) < Duration();
}

PushDimmer::PushDimmer(DimmerDriver & dimmerDriver, uint8_t dimmSpeed)
  : Dimmer(dimmerDriver, dimmSpeed) {}

void PushDimmer::begin_() {
}

bool PushDimmer::update_(bool value)
{
  if (isRising_(value)) {
    if (isDoublePress_()) {
      request(255);
    }
    else {
      toggle();
    }
    lastPinRiseTime_ = Duration();
  }
  lastPinValue_ = value;
}

bool PushDimmer::isRising_(bool pinValue)
{
  return pinValue == true and lastPinValue_ == false;
}

bool PushDimmer::isDoublePress_()
{
  return not (lastPinRiseTime_ + Duration(200) < Duration());
}

} //mys_toolkit
