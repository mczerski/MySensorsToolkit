#ifndef MyTemperatureSensor_h
#define MyTemperatureSensor_h

#include "RequestableValue.h"

#include <DallasTemperature.h>
#include <OneWire.h>

namespace mys_toolkit {

class MyTemperatureSensor : public RequestableValue<float> {
public:
  MyTemperatureSensor(uint8_t pin, uint8_t sensorId, Duration interval) : RequestableValue<float>(sensorId, V_TEMP, S_TEMP, interval), oneWire_(pin), tempSensor_(&oneWire_) {}
private:
  OneWire oneWire_;
  DallasTemperature tempSensor_;
  float readValueCb_() override {
    return tempSensor_.getTempCByIndex(0);
  }
  Duration startMeasurementCb_() override {
    tempSensor_.requestTemperatures();
    return Duration(tempSensor_.millisToWaitForConversion(tempSensor_.getResolution()));
  }
  void begin2_() override {
    tempSensor_.begin();
    tempSensor_.setWaitForConversion(false);
  }
};

} //mys_toolkit

#endif //MyTemperatureSensor_h
