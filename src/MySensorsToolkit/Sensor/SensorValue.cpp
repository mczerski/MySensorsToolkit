#include "SensorValue.h"
#include <MySensorsToolkit/MySensors.h>

namespace mys_toolkit {

void SensorValueBase::present_()
{
  ::present(msg_.getSensor(), sensorType_);
}

void SensorValueBase::forceResend_()
{
  noUpdates_ = FORCE_UPDATE_N_READS;
}

SensorValueBase::SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType)
  : msg_(sensorId, type), noUpdates_(0), sensorType_(sensorType)
{
  if (valuesCount_ < MAX_VALUES)
    values_[valuesCount_++] = this;
}

void SensorValueBase::present()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->present_();
}

void SensorValueBase::forceResend()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->forceResend_();
}

uint8_t SensorValueBase::valuesCount_ = 0;
SensorValueBase* SensorValueBase::values_[];

} //mys_toolkit
