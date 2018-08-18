#include "Dimmer.h"

namespace mys_toolkit {

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
  setLevel_(currentLevel_);
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

bool Dimmer::isRising_(bool pinValue)
{
  return pinValue == true and lastPinValue_ == false;
}

bool Dimmer::isHeldLongEnough_(bool pinValue)
{
  return pinValue == true and lastPinValue_ == true and lastPinRiseTime_ + Duration(2000) < Duration();
}

bool Dimmer::isFalling(bool pinValue)
{
  return pinValue == false and lastPinValue_ == true;
}

bool Dimmer::isLongPress()
{
  return lastPinRiseTime_ + Duration(500) < Duration();
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

void Dimmer::driveLedPin_(uint8_t pin, uint8_t level)
{
  analogWrite(pin, inverted_ ? 255 - level : level);
}

void Dimmer::init_()
{
  setLevel_(currentLevel_);
}

Dimmer::Dimmer(bool inverted, uint8_t dimmSpeed, Functions functions)
  : lastPinValue_(false), state_(OFF), currentLevel_(0),
    requestedLevel_(0), lastLevel_(0), inverted_(inverted),
    dimmSpeed_(min(maxDimmSpeed_, dimmSpeed)),
    functions_(functions)
{
}

Dimmer::~Dimmer()
{
}

bool Dimmer::update(bool value)
{
  if (isRising_(value)) {
    if (not functions_.slowDimming and not functions_.fullBrightness)
      set(state_ == OFF);
    else
      lastPinRiseTime_ = Duration();
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

} //mys_toolkit
