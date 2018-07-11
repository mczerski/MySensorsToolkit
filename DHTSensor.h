#ifndef DHTSensor_h
#define DHTSensor_h

#include "SensorBase.h"
#include <DHT.h>

namespace mys_toolkit {

class DHTSensor: public SensorBase
{
  DHT dhtSensor_;
  uint8_t dataPin_;
  SensorValue<float> humidity_;
  SensorValue<float> temperature_;
  void begin_() override {
    dhtSensor_.setup(dataPin_);
  }
  unsigned long preUpdate_() override {
    return dhtSensor_.getMinimumSamplingPeriod();
  }
  unsigned long update_() override {
    dhtSensor_.readSensor(true);
    humidity_.update(dhtSensor_.getHumidity());
    temperature_.update(dhtSensor_.getTemperature());
    return SLEEP_TIME;
  }
public:
  DHTSensor(uint8_t humSensorId, uint8_t tempSensorId, uint8_t dataPin, float humTreshold = 0, float tempTreshold = 0)
    : dataPin_(dataPin),
      humidity_(humSensorId, V_HUM, S_HUM, humTreshold),
      temperature_(tempSensorId, V_TEMP, S_TEMP, tempTreshold) {}
};

} //mys_toolkit

#endif //DHTSensor_h
