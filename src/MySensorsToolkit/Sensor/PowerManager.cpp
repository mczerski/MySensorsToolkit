#include "PowerManager.h"
#include <MySensorsToolkit/MySensors.h>

namespace mys_toolkit {

int convert2mV(int v)
{
  int voltage_mV = 1.1 * 11 * 1000 * v / 1024; //includes 1/11 divider
  return voltage_mV;
}

uint8_t convertnV2LevelCoincell(int v)
{
  if (v >= 3000)
    return 100;
  else if (v <= 1800)
    return 0;
  else
    return (v - 1800) / ((30 - 18));
}

uint8_t convertnV2LevelLiIon(int v)
{
  if (v >= 4200)
    return 100;
  else if (v <= 3000)
    return 0;
  else
    return (v - 3000) / ((42 - 30));
}

uint8_t convertmV2Level(int v, bool liIonBattery)
{
  if (liIonBattery)
    return convertnV2LevelLiIon(v);
  else
    return convertnV2LevelCoincell(v);
}

PowerManager::PowerManager()
{
}

void PowerManager::handleBatteryLevel_(uint8_t value)
{
  if (abs(int(value) - int(lastBatteryLevel_)) > BATTERY_LEVEL_TRESHOLD or noUpdatesBatteryLevel_ == FORCE_UPDATE_N_READS) {
    lastBatteryLevel_ = value;
    sendBatteryLevel(value);
    noUpdatesBatteryLevel_ = 0;
  } else {
    // Increase no update counter if the humidity stayed the same
    noUpdatesBatteryLevel_++;
  }
}

void PowerManager::handleLowVoltageState_()
{
  auto voltage = convert2mV(analogRead(batteryPin_));
  if (state_ == NORMAL and voltage < 2600)
    state_ = LOW_VOLTAGE_BOOST;
  else if (state_ == LOW_VOLTAGE_BOOST and voltage >= 2800)
    state_ = NORMAL;
}

PowerManager& PowerManager::getInstance()
{
  return instance_;
}

void PowerManager::setupPowerBoost(uint8_t powerBoostPin,  bool initialBoost, bool alwaysBoost, bool lowVoltageBoost)
{
  powerBoostPin_ = powerBoostPin;
  alwaysBoost_ = alwaysBoost;
  lowVoltageBoost_ = lowVoltageBoost;
  if (batteryPin_ != static_cast<uint8_t>(-1) and lowVoltageBoost_) {
    handleLowVoltageState_();
  }
  if (powerBoostPin_ != static_cast<uint8_t>(-1)) {
    pinMode(powerBoostPin_, OUTPUT);
    digitalWrite(powerBoostPin_, initialBoost or alwaysBoost or state_ == LOW_VOLTAGE_BOOST ? HIGH : LOW);
  }
}

void PowerManager::setBatteryPin(uint8_t batteryPin, bool liIonBattery)
{
  batteryPin_ = batteryPin;
  liIonBattery_ = liIonBattery;
  analogReference(INTERNAL);
  //this is to give adc time to setup reference voltage
  analogRead(batteryPin_);
  delay(7);
}

void PowerManager::enterUpdate()
{
  if (powerBoostPin_ == static_cast<uint8_t>(-1) or alwaysBoost_)
      return;

  if (lowVoltageBoost_) {
    handleLowVoltageState_();
    digitalWrite(powerBoostPin_, state_ == LOW_VOLTAGE_BOOST ? HIGH : LOW);
  }
  else
    digitalWrite(powerBoostPin_, HIGH);
  //wait for everything to setup (100ms for dc/dc converter)
  wait(100);
}

void PowerManager::exitUpdate()
{
  if (powerBoostPin_ == static_cast<uint8_t>(-1) or alwaysBoost_)
    return;


  if (lowVoltageBoost_) {
    handleLowVoltageState_();
    digitalWrite(powerBoostPin_, state_ == LOW_VOLTAGE_BOOST ? HIGH : LOW);
  }
  else
    digitalWrite(powerBoostPin_, LOW);
}

void PowerManager::reportBatteryLevel()
{
  if (batteryPin_ == uint8_t(-1))
    return;
  int voltage = convert2mV(analogRead(batteryPin_));
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("V: ");
  Serial.println(voltage);
  #endif
  uint8_t batteryLevel = convertmV2Level(voltage, liIonBattery_);
  handleBatteryLevel_(batteryLevel);
}

void PowerManager::forceResend()
{
  noUpdatesBatteryLevel_ = FORCE_UPDATE_N_READS;
}

PowerManager PowerManager::instance_;


} //mys_toolkit
