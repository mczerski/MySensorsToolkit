#include "MyTemperatureSensor.h"

namespace mys_toolkit {

MyTemperatureSensor::MyTemperatureSensor(uint8_t pin, uint8_t sensorId, Duration interval)
  : RequestableValue<float>(sensorId, V_TEMP, S_TEMP, interval), oneWire_(pin), tempSensor_(&oneWire_)
{
}

float MyTemperatureSensor::readValueCb_()
{
  return tempSensor_.getTempCByIndex(0);
}

Duration MyTemperatureSensor::startMeasurementCb_()
{
  tempSensor_.requestTemperatures();
  return Duration(tempSensor_.millisToWaitForConversion(tempSensor_.getResolution()));
}

void MyTemperatureSensor::begin2_()
{
  tempSensor_.begin();
  tempSensor_.setWaitForConversion(false);
}

} //mys_toolkit
