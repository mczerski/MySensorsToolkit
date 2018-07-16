#include "SensorValue.h"
#include <MySensorsToolkit/MySensors.h>

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
  somethingSent_ = false;
}

static void SensorValueBase::update(bool success, bool somethingSent)
{
  success_ &= success;
  somethingSent_ |= somethingSent;
}

static bool SensorValueBase::wasSuccess()
{
  return success_;
}

static bool SensorValueBase::wasSomethingSent() {
  return somethingSent_;
}

uint8_t SensorValueBase::valuesCount_ = 0;
SensorValueBase* SensorValueBase::values_[];
bool SensorValueBase::success_ = true;
bool SensorValueBase::somethingSent_ = false;

} //mys_toolkit
