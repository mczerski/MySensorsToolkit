#pragma once

#include <core/MyMessage.h>

namespace mys_toolkit {

class SensorBase {
  static constexpr uint8_t MYS_TOOLKIT_INTERRUPT_NOT_DEFINED = 255;
  static constexpr uint8_t MYS_TOOLKIT_MODE_NOT_DEFINED = 255;
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static SensorBase* sensors_[MAX_SENSORS];
  static uint8_t consecutiveFails_;
  static uint8_t buttonPin_;
  static uint8_t ledPin_;
  static uint8_t interruptPin_;
  static uint8_t interruptMode_;
  static bool alwaysBoostOn_;
  static const uint8_t N_RETRIES = 14;
  static const unsigned long UPDATE_INTERVAL = 1000;
  static unsigned long now_;
  static unsigned long lastUpdate_;
  bool initialised_ = false;
  static void setLed_(uint8_t value);

  virtual bool begin_();
  virtual unsigned long preUpdate_();
  virtual unsigned long update_();

  static unsigned long getSleepTimeout_(bool success, unsigned long sleep=0);

protected:
  // Sleep time between sensor updates (in milliseconds)
  static constexpr unsigned long SLEEP_TIME = 600000;

  void requestInterrupt(uint8_t pin, uint8_t mode);

public:
  SensorBase();
  static void present();
  static void begin(uint8_t batteryPin = -1, bool liIonBattery = false, uint8_t powerBoostPin = -1,
                    bool initialBoostOn = false, bool alwaysBoostOn = false, uint8_t buttonPin = MYS_TOOLKIT_INTERRUPT_NOT_DEFINED,
                    uint8_t ledPin = -1);
  static void update();
  static void receive(const MyMessage &message);
};

} //mys_toolkit

