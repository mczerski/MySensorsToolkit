#ifndef MyTemperatureSensor_h
#define MyTemperatureSensor_h

#include "RequestableValue.h"

#include <DallasTemperature.h>
#include <OneWire.h>

namespace mys_toolkit {

//TODO: rename
class MyTemperatureSensor : public RequestableValue<float> {
public:
  MyTemperatureSensor(uint8_t pin, uint8_t sensorId, Duration interval);

private:
  OneWire oneWire_;
  DallasTemperature tempSensor_;
  float readValueCb_() override;
  Duration startMeasurementCb_() override;
  void begin2_() override;
};

} //mys_toolkit

#endif //MyTemperatureSensor_h
