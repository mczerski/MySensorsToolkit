#include "Parameter.h"
#include "SensorValue.h"
#include "SensorBase.h"
#include "PowerManager.h"
#include <MySensorsToolkit/utils.h>
#include <MySensorsToolkit/Message.h>
#include <MySensorsToolkit/MySensors.h>
                    
#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

#ifndef digitalPinToInterrupt
#define digitalPinToInterrupt(p) (p)
#endif

namespace mys_toolkit {

void SensorBase::setLed_(uint8_t value)
{
  if (ledPin_ != uint8_t(-1))
    digitalWrite(ledPin_, value);
}

bool SensorBase::begin_()
{
  return true;
}

unsigned long SensorBase::preUpdate_()
{
  return 0;
}

unsigned long SensorBase::update_()
{
  return SLEEP_TIME;
}

unsigned long SensorBase::getSleepTimeout_(bool success, unsigned long sleep)
{
  if (!success) {
    if (consecutiveFails_ < N_RETRIES) {
      consecutiveFails_++;
    }
  }
  else {
    consecutiveFails_ = 0;
  }
  unsigned long sleepTimeout = consecutiveFails_ ? (1<<(consecutiveFails_-1))*UPDATE_INTERVAL : sleep;
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("Sleep: ");
  MYS_TOOLKIT_SERIAL.println(sleepTimeout);
  wait(500);
  #endif
  return sleepTimeout;
}

void SensorBase::requestInterrupt(uint8_t pin, uint8_t mode)
{
  if (interruptPin_ == MYS_TOOLKIT_INTERRUPT_NOT_DEFINED) {
    interruptPin_ = pin;
    interruptMode_ = mode;
  }
}

SensorBase::SensorBase()
{
  if (sensorsCount_ < MAX_SENSORS)
    sensors_[sensorsCount_++] = this;
}

void SensorBase::present()
{
  SensorValueBase::present();
  ParameterBase::present();
}
void SensorBase::begin(uint8_t batteryPin, bool liIonBattery, uint8_t powerBoostPin,
                       bool initialBoost, bool alwaysBoost, bool lowVoltageBoost,
                       uint8_t buttonPin, uint8_t ledPin)
{
  if (buttonPin != uint8_t(-1)) {
    buttonPin_ = buttonPin;
    pinMode(buttonPin_, INPUT_PULLUP);
  }

  ledPin_ = ledPin;
  if (ledPin_ != uint8_t(-1))
    pinMode(ledPin_, OUTPUT);
  setLed_(LOW);

  PowerManager::getInstance().setBatteryPin(batteryPin, liIonBattery);
  PowerManager::getInstance().setupPowerBoost(powerBoostPin, initialBoost, alwaysBoost, lowVoltageBoost);
  for (size_t i=0; i<sensorsCount_; i++)
    sensors_[i]->initialised_ = sensors_[i]->begin_();
}

void SensorBase::update()
{
  PowerManager::getInstance().enterUpdate();

  unsigned long maxWait = 0;
  for (size_t i=0; i<sensorsCount_; i++) {
    if (sensors_[i]->initialised_) {
      auto wait = sensors_[i]->preUpdate_();
      maxWait = max(maxWait, wait);
    }
  }
  wait(maxWait);

  unsigned long minWait = -1;
  for (size_t i=0; i<sensorsCount_; i++) {
    if (sensors_[i]->initialised_) {
      auto wait = sensors_[i]->update_();
      minWait = min(minWait, wait);
    }
  }
  if (minWait == static_cast<unsigned long>(-1))
    minWait = SLEEP_TIME;

  checkTransport();
  auto result = Message::sendAll();
  PowerManager::getInstance().reportBatteryLevel();

  unsigned long sleepTimeout = getSleepTimeout_(result.success, minWait);

  setLed_(HIGH);

  PowerManager::getInstance().exitUpdate();

  int wakeUpCause;
  static bool wakeupFromButton = false;
  auto smartSleep = wakeupFromButton;
  if (buttonPin_ == MYS_TOOLKIT_INTERRUPT_NOT_DEFINED)
    wakeUpCause = sleep(digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout, smartSleep);
  else
    wakeUpCause = sleep(digitalPinToInterrupt(buttonPin_), FALLING, digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout, smartSleep);
  wakeupFromButton = false;
  if (buttonPin_ != MYS_TOOLKIT_INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(buttonPin_)) {
    setLed_(LOW);
    SensorValueBase::forceResendAll();
    wakeupFromButton = true;
    #ifdef MYS_TOOLKIT_DEBUG
    MYS_TOOLKIT_SERIAL.println("Wake up from button");
    #endif
  }
  else if (interruptPin_ != MYS_TOOLKIT_INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(interruptPin_)) {
    setLed_(LOW);
    #ifdef MYS_TOOLKIT_DEBUG
    MYS_TOOLKIT_SERIAL.println("Wake up from sensor");
    #endif
  }
}

void SensorBase::receive(const MyMessage &message) {
  ParameterBase::receive(message);
}

uint8_t SensorBase::sensorsCount_ = 0;
SensorBase * SensorBase::sensors_[];
uint8_t SensorBase::consecutiveFails_ = 0;
uint8_t SensorBase::buttonPin_ = MYS_TOOLKIT_INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::ledPin_;
uint8_t SensorBase::interruptPin_ = MYS_TOOLKIT_INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::interruptMode_ = MYS_TOOLKIT_MODE_NOT_DEFINED;

} //mys_toolkit
