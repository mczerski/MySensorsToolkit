#ifndef PowerManager_h
#define PowerManager_h

#include <Arduino.h>

namespace mys_toolkit {

class PowerManager {
  static constexpr uint8_t FORCE_UPDATE_N_READS = 10;
  static constexpr int BATTERY_LEVEL_TRESHOLD = 5;
  uint8_t powerBoostPin_ = -1;
  uint8_t batteryPin_ = -1;
  uint8_t lastBatteryLevel_ = -1;
  uint8_t noUpdatesBatteryLevel_ = 0;
  bool liIonBattery_ = false;
  static PowerManager instance_;
  PowerManager();
  void handleBatteryLevel_(uint8_t value);
public:
  static PowerManager& getInstance();
  void setupPowerBoost(uint8_t powerBoostPin = -1,  bool initialBoostOn = false);
  void setBatteryPin(uint8_t batteryPin, bool liIonBattery = false);
  void turnBoosterOn();
  void turnBoosterOff();
  void reportBatteryLevel();
  void forceResend();
};

} //mys_toolkit

#endif //PowerManager_h
