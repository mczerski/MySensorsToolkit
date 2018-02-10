#ifndef MyMySensor_h
#define MyMySensor_h

#include "MyMySensors.h"

namespace mymysensors {

class MyMySensor {
protected:
  MyMessage msg_;
  uint8_t noUpdates_;
  uint8_t sensorId_;
  uint8_t sensorType_;
  
  static const uint8_t FORCE_UPDATE_N_READS = 10;
  // Sleep time between sensor updates (in milliseconds)
  static const uint64_t SLEEP_TIME = 60000;
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static MyMySensor* sensors_[MAX_SENSORS];
  static PowerManager* powerManager_;
  static uint8_t consecutiveFails_;
  static uint8_t interruptPin_;
  static uint8_t interruptMode_;
  static const uint8_t N_RETRIES = 14;
  static const unsigned long UPDATE_INTERVAL = 1000;

  virtual void begin_() {};
  virtual bool update_() = 0;
  virtual unsigned long preUpdate_() = 0;
  void forceResend_() {
    noUpdates_ = FORCE_UPDATE_N_READS;
  }
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

  void requestInterrupt(uint8_t pin, uint8_t mode) {
    if (interruptPin_ == INTERRUPT_NOT_DEFINED) {
      interruptPin_ = pin;
      interruptMode_ = mode;
    }
  }

public:
  MyMySensor(uint8_t sensorId, uint8_t type, uint8_t sensorType)
    : msg_(sensorId, type), noUpdates_(0), sensorId_(sensorId), sensorType_(sensorType)
  {
    if (sensorsCount_ < MAX_SENSORS)
      sensors_[sensorsCount_++] = this;
  }
  static void present() {
    for (size_t i=0; i<sensorsCount_; i++)
      ::present(sensors_[i]->sensorId_, sensors_[i]->sensorType_);
  }
  static void begin(uint8_t batteryPin = -1, bool liIonBattery = false, uint8_t powerBoostPin = -1,  bool initialBoostOn = false) {
    pinMode(MY_LED, OUTPUT);
    digitalWrite(MY_LED, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    powerManager_ = &PowerManager::getInstance();
    powerManager_->setupPowerBoost(powerBoostPin, initialBoostOn);
    powerManager_->setBatteryPin(batteryPin, liIonBattery);
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->begin_();
  }
  static void update() {
    powerManager_->turnBoosterOn();
    //wait for everything to setup (100ms for dc/dc converter)
    wait(100);

    unsigned long maxWait = 0;
    for (size_t i=0; i<sensorsCount_; i++) {
      auto wait = sensors_[i]->preUpdate_();
      maxWait = max(maxWait, wait);
    }
    wait(maxWait);

	checkTransport();

    bool success = true;
    for (size_t i=0; i<sensorsCount_; i++)
      success &= sensors_[i]->update_();

	powerManager_->reportBatteryLevel();
	unsigned long sleepTimeout = getSleepTimeout_(success, SLEEP_TIME);

    digitalWrite(MY_LED, HIGH);

    int wakeUpCause = sleep(digitalPinToInterrupt(BUTTON_PIN), FALLING, digitalPinToInterrupt(interruptPin_), interruptMode_, sleepTimeout);
    if (wakeUpCause == digitalPinToInterrupt(BUTTON_PIN)) {
      digitalWrite(MY_LED, LOW);
      for (size_t i=0; i<sensorsCount_; i++)
        sensors_[i]->forceResend_();
      #ifdef MY_MY_DEBUG
      Serial.println("Wake up from button");
      #endif
    }
    else if (wakeUpCause == digitalPinToInterrupt(interruptPin_)) {
      digitalWrite(MY_LED, LOW);
      #ifdef MY_MY_DEBUG
      Serial.println("Wake up from sensor");
      #endif
    }
  }
};

uint8_t MyMySensor::sensorsCount_ = 0;
MyMySensor * MyMySensor::sensors_[];
uint8_t MyMySensor::consecutiveFails_ = 0;
uint8_t MyMySensor::interruptPin_ = INTERRUPT_NOT_DEFINED;
uint8_t MyMySensor::interruptMode_ = MODE_NOT_DEFINED;
PowerManager* MyMySensor::powerManager_ = nullptr;

template <typename ValueType>
class MyMySensorBase : public MyMySensor {
  ValueType lastValue_;
  ValueType treshold_;

  virtual ValueType getValue_() = 0;
  bool update_() override {
    return handleValue_(getValue_());
  }
  bool handleValue_(ValueType value) {
    bool success = true;

    if (abs(lastValue_ - value) > treshold_ || noUpdates_ == FORCE_UPDATE_N_READS) {
      lastValue_ = value;

      success = sendMessage_(value);
      if (success) {
        noUpdates_ = 0;
      }
      else {
        noUpdates_ = FORCE_UPDATE_N_READS;
        #ifdef MY_MY_DEBUG
        Serial.print("t=");
        Serial.print(msg_.type);
        Serial.print(",c=");
        Serial.print(msg_.sensor);
        Serial.println(". Send failed");
        #endif
      }
    } else {
      noUpdates_++;
    }
    return success;
  }
  bool sendMessage_(ValueType value) {
    setMessageValue_(msg_, value);
    return send(msg_, true) and wait(2000, C_SET, msg_.type);
  }
public:
  MyMySensorBase(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType treshold = 0)
    : MyMySensor(sensorId, type, sensorType), lastValue_(-1), treshold_(treshold) {}
};

} // mymysensors

#endif //MyMySensor_h
