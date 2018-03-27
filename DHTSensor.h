#ifndef DHTSensor_h
#define DHTSensor_h

#include "MyMySensor.h"
#include <DHT.h>

namespace mymysensors {


class DHTSensor: public MyMySensor
{
  DHT dhtSensor_;
  uint8_t dataPin_;
  MyValue<float> humidity_;
  MyValue<float> temperature_;
  void begin_() override {
    dhtSensor_.setup(dataPin_);
  }
  unsigned long preUpdate_() override {
    return dhtSensor_.getMinimumSamplingPeriod();
  }
  void update_() override {
    dhtSensor_.readSensor(true);
    humidity_.update(dhtSensor_.getHumidity());
    temperature_.update(dhtSensor_.getTemperature());
  }
public:
  DHTSensor(uint8_t humSensorId, uint8_t tempSensorId, uint8_t dataPin, float humTreshold = 0, float tempTreshold = 0)
    : dataPin_(dataPin),
      humidity_(humSensorId, V_HUM, S_HUM, humTreshold),
      temperature_(tempSensorId, V_TEMP, S_TEMP, tempTreshold) {}
};

} // mymysensors

#endif //DHTSensor_h
