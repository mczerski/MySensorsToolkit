#include "SensorValue.h"

extern bool present(const uint8_t sensorId, const uint8_t sensorType, const char *description="",
                    const bool ack = false);

namespace mys_toolkit {

void SensorValueBase::present_()
{
  ::present(sensorId_, sensorType_);
}

void SensorValueBase::forceResend_()
{
  noUpdates_ = FORCE_UPDATE_N_READS;
}

SensorValueBase::SensorValueBase(uint8_t sensorId, uint8_t type, uint8_t sensorType)
  : msg_(sensorId, type), noUpdates_(0), sensorId_(sensorId), sensorType_(sensorType)
{
  if (valuesCount_ < MAX_VALUES)
    values_[valuesCount_++] = this;
}

static void SensorValueBase::present()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->present_();
}

static void SensorValueBase::forceResend()
{
  for (size_t i=0; i<valuesCount_; i++)
    values_[i]->forceResend_();
}

static void SensorValueBase::beforeUpdate()
{
  success_ = true;
}

static void SensorValueBase::update(bool success)
{
  success_ &= success;
}

static bool SensorValueBase::afterUpdate()
{
  return success_;
}

uint8_t SensorValueBase::valuesCount_ = 0;
SensorValueBase* SensorValueBase::values_[];
bool SensorValueBase::success_ = true;

} //mys_toolkit
