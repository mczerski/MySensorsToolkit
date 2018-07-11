#ifndef SensorBase_h
#define SensorBase_h

#include "PowerManager.h"
#include "SensorValue.h"
#include "utils.h"

namespace mys_toolkit {

class SensorBase {
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static SensorBase* sensors_[MAX_SENSORS];
  static PowerManager* powerManager_;
  static uint8_t consecutiveFails_;
  static uint8_t buttonPin_;
  static uint8_t interruptPin_;
  static uint8_t interruptMode_;
  static bool alwaysBoostOn_;
  static const uint8_t N_RETRIES = 14;
  static const unsigned long UPDATE_INTERVAL = 1000;
  static unsigned long now_;
  static unsigned long lastUpdate_;

  virtual void begin_() {};
  virtual unsigned long preUpdate_() {return 0;};
  virtual unsigned long update_() {return SLEEP_TIME;};

  static unsigned long getSleepTimeout_(bool success, unsigned long sleep = 0) {
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

protected:
  // Sleep time between sensor updates (in milliseconds)
  static constexpr unsigned long SLEEP_TIME = 600000;

  void requestInterrupt(uint8_t pin, uint8_t mode) {
    if (interruptPin_ == INTERRUPT_NOT_DEFINED) {
      interruptPin_ = pin;
      interruptMode_ = mode;
    }
  }

public:
  SensorBase()
  {
    if (sensorsCount_ < MAX_SENSORS)
      sensors_[sensorsCount_++] = this;
  }

  static void present() {
    SensorValueBase::present();
  }
  static void begin(uint8_t batteryPin = -1, bool liIonBattery = false, uint8_t powerBoostPin = -1,
                    bool initialBoostOn = false, bool alwaysBoostOn = false, uint8_t buttonPin = INTERRUPT_NOT_DEFINED) {
    alwaysBoostOn_ = alwaysBoostOn;
    if (buttonPin != uint8_t(-1)) {
      buttonPin_ = buttonPin;
      pinMode(buttonPin_, INPUT_PULLUP);
    }

    pinMode(MY_LED, OUTPUT);
    digitalWrite(MY_LED, LOW);

    powerManager_ = &PowerManager::getInstance();
    powerManager_->setupPowerBoost(powerBoostPin, initialBoostOn or alwaysBoostOn_);
    powerManager_->setBatteryPin(batteryPin, liIonBattery);
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->begin_();
  }
  static void update() {
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

    digitalWrite(MY_LED, HIGH);
 
    if (not alwaysBoostOn_)
      powerManager_->turnBoosterOff();

    int wakeUpCause;
    if (buttonPin_ == INTERRUPT_NOT_DEFINED)
      wakeUpCause = sleep(digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout);
    else
      wakeUpCause = sleep(digitalPinToInterrupt(buttonPin_), FALLING, digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout);

    if (buttonPin_ != INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(buttonPin_)) {
      digitalWrite(MY_LED, LOW);
      SensorValueBase::forceResend();
      #ifdef MY_MY_DEBUG
      Serial.println("Wake up from button");
      #endif
    }
    else if (interruptPin_ != INTERRUPT_NOT_DEFINED and wakeUpCause == digitalPinToInterrupt(interruptPin_)) {
      digitalWrite(MY_LED, LOW);
      #ifdef MY_MY_DEBUG
      Serial.println("Wake up from sensor");
      #endif
    }
  }

};

uint8_t SensorBase::sensorsCount_ = 0;
SensorBase * SensorBase::sensors_[];
uint8_t SensorBase::consecutiveFails_ = 0;
uint8_t SensorBase::buttonPin_ = INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::interruptPin_ = INTERRUPT_NOT_DEFINED;
uint8_t SensorBase::interruptMode_ = MODE_NOT_DEFINED;
bool SensorBase::alwaysBoostOn_ = false;
PowerManager* SensorBase::powerManager_ = nullptr;

} //mys_toolkit

#endif //SensorBase_h
