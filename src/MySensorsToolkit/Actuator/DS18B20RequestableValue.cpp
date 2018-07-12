#include "DS18B20RequestableValue.h"

namespace mys_toolkit {

DS18B20RequestableValue::DS18B20RequestableValue(uint8_t pin, uint8_t sensorId, Duration interval)
  : RequestableValue<float>(sensorId, V_TEMP, S_TEMP, interval), oneWire_(pin), tempSensor_(&oneWire_)
{
}

float DS18B20RequestableValue::readValueCb_()
{
  return tempSensor_.getTempCByIndex(0);
}

Duration DS18B20RequestableValue::startMeasurementCb_()
{
  tempSensor_.requestTemperatures();
  return Duration(tempSensor_.millisToWaitForConversion(tempSensor_.getResolution()));
}

void DS18B20RequestableValue::begin2_()
{
  tempSensor_.begin();
  tempSensor_.setWaitForConversion(false);
}

} //mys_toolkit
