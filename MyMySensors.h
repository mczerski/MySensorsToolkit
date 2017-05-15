#ifndef MyMySensors_h
#define MyMySensors_h

#include <MySensors.h>

namespace mymysensors {

static const uint8_t FORCE_UPDATE_N_READS = 10;
static const unsigned long UPDATE_INTERVAL = 1000;
static const uint8_t N_RETRIES = 14;

uint8_t consecutiveFails = 0;

int convert2mV(int v) {
  int voltage_mV = v * 1.1 * 1.1 * 1000 * 10 / 1024; //includes 1/11 divider
  return voltage_mV;
}

uint8_t convertnV2LevelCoincell(int v) {
  if (v >= 3000)
    return 100;
  else if (v <= 1800)
    return 0;
  else
    return (v - 1800) / ((30 - 18));
}

uint8_t convertnV2LevelLiIon(int v) {
  if (v >= 4200)
    return 100;
  else if (v <= 3000)
    return 0;
  else
    return (v - 3000) / ((42 - 30));
}

uint8_t convertmV2Level(int v, bool liIonBattery) {
  if (liIonBattery)
    return convertnV2LevelLiIon(v);
  else
    return convertnV2LevelCoincell(v);
}

template <typename ValueType>
bool sendValue(MyMessage &msg, ValueType value) {
  return send(msg.set(value), true);
}

template <>
bool sendValue(MyMessage &msg, float value) {
  return send(msg.set(value, 1), true);
}

template <typename ValueType>
bool handleValue(ValueType value, ValueType &lastValue, uint8_t &noUpdatesValue, MyMessage &msg) {
  bool success = true;
  if (value != lastValue)
    consecutiveFails = 0;

  if (lastValue != value || noUpdatesValue == FORCE_UPDATE_N_READS) {
    lastValue = value;

    success = sendValue(msg, value);
    if (success) {
      noUpdatesValue = 0;
    }
    else {
      noUpdatesValue = FORCE_UPDATE_N_READS;
      #ifdef MY_DEBUG2
      Serial.println("Send failed");
      #endif
    }
  } else {
    noUpdatesValue++;
  }
  return success;
}

void handleBatteryLevel(uint8_t value, uint8_t &lastValue, uint8_t &noUpdatesValue) {
  if (value != lastValue || noUpdatesValue == FORCE_UPDATE_N_READS) {
    lastValue = value;
    sendBatteryLevel(value);
    noUpdatesValue = 0;
  } else {
    // Increase no update counter if the humidity stayed the same
    noUpdatesValue++;
  }
}

class PowerManager {
  uint8_t powerBoostPin_ = -1;
  uint8_t batteryPin_ = -1;
  uint8_t lastBatteryLevel_ = -1;
  uint8_t noUpdatesBatteryLevel_ = 0;
  bool liIonBattery_ = false;
  static PowerManager instance_;
  PowerManager() {}
public:
  static PowerManager& initInstance(uint8_t powerBoostPin,  bool initialBoostOn) {
    if (powerBoostPin != uint8_t(-1)) {
      instance_.powerBoostPin_ = powerBoostPin;
      pinMode(powerBoostPin, OUTPUT);
      digitalWrite(powerBoostPin, initialBoostOn);
    }
    return instance_;
  }
  static PowerManager& getInstance() {
    return instance_;
  }
  void setBatteryPin(uint8_t batteryPin, bool liIonBattery = false) {
    batteryPin_ = batteryPin;
    liIonBattery_ = liIonBattery;
    analogReference(INTERNAL);
  }
  void turnBoosterOn() {
    digitalWrite(powerBoostPin_, HIGH);
  }
  void turnBoosterOff() {
    digitalWrite(powerBoostPin_, LOW);
  }
  void reportBatteryLevel() {
    if (batteryPin_ == uint8_t(-1))
      return;
    int voltage = convert2mV(analogRead(batteryPin_));
    #ifdef MY_DEBUG2
    Serial.print("V: ");
    Serial.println(voltage);
    #endif
    uint8_t batteryLevel = convertmV2Level(voltage, liIonBattery_);
    handleBatteryLevel(batteryLevel, lastBatteryLevel_, noUpdatesBatteryLevel_);
  }
  void forceResend() {
    noUpdatesBatteryLevel_ = FORCE_UPDATE_N_READS;
  }
};

PowerManager PowerManager::instance_;

unsigned long getSleepTimeout(bool success, unsigned long sleep = 0) {
  PowerManager::getInstance().reportBatteryLevel();
  if (!success) {
    if (consecutiveFails < N_RETRIES) {
      consecutiveFails++;
    }
  }
  else {
    consecutiveFails = 0;
  }
  unsigned long sleepTimeout = consecutiveFails ? (1<<(consecutiveFails-1))*UPDATE_INTERVAL : sleep;
  #ifdef MY_DEBUG2
  Serial.print("Sleep: ");
  Serial.println(sleepTimeout);
  wait(500);
  #endif
  return sleepTimeout;
}

void checkTransport() {
  for (int i=0; i<20; i++) {
    if (isTransportReady())
      break;
    wait(500); // transport is not operational, allow the transport layer to fix this
  }
}

template <typename ValueType>
class MyValue {
  MyMessage msg_;
  ValueType last_;
  uint8_t noUpdates_;
  uint8_t childId_;
  uint8_t sensorType_;
public:
  MyValue(uint8_t sensor, uint8_t type, uint8_t sensorType)
    : msg_(sensor, type), last_(-1), noUpdates_(0), childId_(sensor), sensorType_(sensorType) {}
  void presentValue() {
    present(childId_, sensorType_);
  }
  bool updateValue(ValueType current, bool force=false) {
    if (force)
      forceResend();
    return handleValue(current, last_, noUpdates_, msg_);
  }
  void forceResend() {
    noUpdates_ = FORCE_UPDATE_N_READS;
  }
};

} // mymysensors

#endif //MyMySensors_h
