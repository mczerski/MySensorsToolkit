#ifndef DS18B20RequestableValue_h
#define DS18B20RequestableValue_h

#include "RequestableValue.h"

#include <DallasTemperature.h>
#include <OneWire.h>

namespace mys_toolkit {

class DS18B20RequestableValue : public RequestableValue<float> {
public:
  DS18B20RequestableValue(uint8_t pin, uint8_t sensorId, Duration interval);

private:
  OneWire oneWire_;
  DallasTemperature tempSensor_;
  float readValueCb_() override;
  Duration startMeasurementCb_() override;
  void begin2_() override;
};

} //mys_toolkit

#endif //DS18B20RequestableValue_h
