#pragma once

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
  bool alwaysBoost_ = false;
  bool lowVoltageBoost_ = false;
  enum State {
      NORMAL,
      LOW_VOLTAGE_BOOST
  };
  State state_ = NORMAL;
  static PowerManager instance_;
  PowerManager();
  void handleBatteryLevel_(uint8_t value);
  void handleLowVoltageState_();
public:
  static PowerManager& getInstance();
  void setupPowerBoost(uint8_t powerBoostPin = -1,  bool initialBoostOn = false, bool alwaysBoost = false, bool lowVoltageBoost = false);
  void setBatteryPin(uint8_t batteryPin, bool liIonBattery = false);
  void enterUpdate();
  void exitUpdate();
  void reportBatteryLevel();
  void forceResend();
};

} //mys_toolkit

