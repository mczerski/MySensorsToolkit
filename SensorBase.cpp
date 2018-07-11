#include "SensorBase.h"
#include "SensorValue.h"
#include "utils.h"

extern void wait(const uint32_t);
extern int8_t sleep(const uint8_t interrupt, const uint8_t mode, const uint32_t sleepingMS = 0,
                    const bool smartSleep = false);
extern int8_t sleep(const uint8_t interrupt1, const uint8_t mode1, const uint8_t interrupt2,
                    const uint8_t mode2, const uint32_t sleepingMS = 0, const bool smartSleep = false);
                    
namespace mys_toolkit {

void SensorBase::begin_()
{
}

unsigned long SensorBase::preUpdate_()
{
  return 0;
}

unsigned long SensorBase::update_()
{
  return SLEEP_TIME;
}

static unsigned long SensorBase::getSleepTimeout_(bool success, unsigned long sleep)
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
  #ifdef MY_MY_DEBUG
  Serial.print("Sleep: ");
  Serial.println(sleepTimeout);
  wait(500);
  #endif
  return sleepTimeout;
}

void SensorBase::requestInterrupt(uint8_t pin, uint8_t mode)
{
  if (interruptPin_ == INTERRUPT_NOT_DEFINED) {
    interruptPin_ = pin;
    interruptMode_ = mode;
  }
}

SensorBase::SensorBase()
{
  if (sensorsCount_ < MAX_SENSORS)
    sensors_[sensorsCount_++] = this;
}

static void SensorBase::present()
{
  SensorValueBase::present();
}
static void SensorBase::begin(uint8_t batteryPin, bool liIonBattery, uint8_t powerBoostPin,
                              bool initialBoostOn, bool alwaysBoostOn, uint8_t buttonPin, uint8_t ledPin)
{
  alwaysBoostOn_ = alwaysBoostOn;
  if (buttonPin != uint8_t(-1)) {
    buttonPin_ = buttonPin;
    pinMode(buttonPin_, INPUT_PULLUP);
  }

  ledPin_ = ledPin;
  pinMode(ledPin_, OUTPUT);
  digitalWrite(ledPin_, LOW);

  powerManager_ = &PowerManager::getInstance();
  powerManager_->setupPowerBoost(powerBoostPin, initialBoostOn or alwaysBoostOn_);
  powerManager_->setBatteryPin(batteryPin, liIonBattery);
  for (size_t i=0; i<sensorsCount_; i++)
    sensors_[i]->begin_();
}

static void SensorBase::update()
{
  if (not alwaysBoostOn_) {
    powerManager_->turnBoosterOn();
    //wait for everything to setup (100ms for dc/dc converter)
    wait(100);
  }

  unsigned long maxWait = 0;
  for (size_t i=0; i<sensorsCount_; i++) {
    auto wait = sensors_[i]->preUpdate_();
    maxWait = max(maxWait, wait);
  }
  wait(maxWait);

  checkTransport();

  SensorValueBase::beforeUpdate();

  unsigned long minWait = -1;
  for (size_t i=0; i<sensorsCount_; i++) {
    auto wait = sensors_[i]->update_();
    minWait = min(minWait, wait);
  }
  bool success = SensorValueBase::afterUpdate();

  powerManager_->reportBatteryLevel();
  unsigned long sleepTimeout = getSleepTimeout_(success, minWait);

  digitalWrite(ledPin_, HIGH);

  if (not alwaysBoostOn_)
    powerManager_->turnBoosterOff();

  int wakeUpCause;
  if (buttonPin_ == INTERRUPT_NOT_DEFINED)
    wakeUpCause = sleep(digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout);
  else
    wakeUpCause = sleep(digitalPinToInterrupt(buttonPin_), FALLING, digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout);

  if (buttonPin_ != INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(buttonPin_)) {
    digitalWrite(ledPin_, LOW);
    SensorValueBase::forceResend();
    #ifdef MY_MY_DEBUG
    Serial.println("Wake up from button");
    #endif
  }
  else if (interruptPin_ != INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(interruptPin_)) {
    digitalWrite(ledPin_, LOW);
    #ifdef MY_MY_DEBUG
    Serial.println("Wake up from sensor");
    #endif
  }
}

uint8_t SensorBase::sensorsCount_ = 0;
SensorBase * SensorBase::sensors_[];
uint8_t SensorBase::consecutiveFails_ = 0;
uint8_t SensorBase::buttonPin_ = INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::ledPin_;
uint8_t SensorBase::interruptPin_ = INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::interruptMode_ = MODE_NOT_DEFINED;
bool SensorBase::alwaysBoostOn_ = false;
PowerManager* SensorBase::powerManager_ = nullptr;

} //mys_toolkit
