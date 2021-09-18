#include "SensorValue.h"
#include <MySensorsToolkit/MySensors.h>

namespace mys_toolkit {

void SensorValueBase::present_()
{
  ::present(msg_.getSensor(), sensorType_);
}

void SensorValueBase::forceResend()
{
  forceResend_ = true;
}

SensorValueBase::SensorValueBase(uint8_t sensorId, mysensors_data_t type, mysensors_sensor_t sensorType)
  : msg_(sensorId, type), sensorType_(sensorType)
{
  if (valuesCount_ < MAX_VALUES)
    values_[valuesCount_++] = this;
}

void SensorValueBase::present()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->present_();
}

void SensorValueBase::forceResendAll()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->forceResend();
}

uint8_t SensorValueBase::valuesCount_ = 0;
SensorValueBase* SensorValueBase::values_[];

} //mys_toolkit
