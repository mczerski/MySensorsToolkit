#ifndef MyTemperatureSensor_h
#define MyTemperatureSensor_h

#include "MyRequestingValue.h"

#include <DallasTemperature.h>
#include <OneWire.h>

namespace mymysensors {

class MyTemperatureSensor : public MyRequestingValue<float> {
public:
  MyTemperatureSensor(uint8_t pin, uint8_t sensorId, MyDuration interval) : MyRequestingValue<float>(sensorId, V_TEMP, S_TEMP, interval), oneWire_(pin), tempSensor_(&oneWire_) {}
private:
  OneWire oneWire_;
  DallasTemperature tempSensor_;
  float readValueCb_() override {
    return tempSensor_.getTempCByIndex(0);
  }
  MyDuration startMeasurementCb_() override {
    tempSensor_.requestTemperatures();
    return MyDuration(tempSensor_.millisToWaitForConversion(tempSensor_.getResolution()));
  }
  void begin2_() override {
    tempSensor_.begin();
    tempSensor_.setWaitForConversion(false);
  }
};

} // mymysensors

#endif //MyTemperatureSensor_h
