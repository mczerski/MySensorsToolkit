#include "DimmerActuator.h"

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

uint8_t DimmerActuator::fromPercentage_(uint8_t percentage)
{
  if (percentage == 1) {
    return 1;
  }
  return uint8_t(round(255.0*percentage/100));
}

uint8_t DimmerActuator::fromLevel_(uint8_t level)
{
  if (level >= 1 and level <= 2) {
      return 1;
  }
  return uint8_t(round(100.0*level/255));
}

void DimmerActuator::sendCurrentLevel_()
{
  uint8_t percentage = fromLevel_(dim_.getLevel());
  dimmerMsg_.send(percentage);
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("sendCurrentLevel ");
  MYS_TOOLKIT_SERIAL.print(percentage);
  MYS_TOOLKIT_SERIAL.print(" for child id ");
  MYS_TOOLKIT_SERIAL.println(dimmerMsg_.getSensor());
  #endif
}

void DimmerActuator::begin_()
{
  dim_.begin();
}

void DimmerActuator::firstUpdate_()
{
  sendCurrentLevel_();
}

void DimmerActuator::update_()
{
  if (dim_.update(sw_.update()))
    sendCurrentLevel_();
}

void DimmerActuator::receive_(const MyMessage &message)
{
  if (mGetCommand(message) == C_REQ) {
    sendCurrentLevel_();
  }
  else if (mGetCommand(message) == C_SET and message.type == V_DIMMER) {
    //  Retrieve the power or dim level from the incoming request message
    int requestedValue = atoi(message.data);

    if (requestedValue == 255) {
      dim_.set(true);
    }
    else {
      // Clip incoming level to valid range of 0 to 100
      requestedValue = requestedValue > 100 ? 100 : requestedValue;
      requestedValue = requestedValue < 0   ? 0   : requestedValue;
      dim_.request(fromPercentage_(requestedValue));
    }
  }
}

DimmerActuator::DimmerActuator(uint8_t sensorId, Dimmer &dim, Switch &sw)
  : ActuatorBase(sensorId, S_DIMMER),
    dim_(dim),
    sw_(sw),
    dimmerMsg_(sensorId, V_DIMMER)
{}

} //mys_toolkit
