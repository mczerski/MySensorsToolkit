#ifndef BH1750Sensor_h
#define BH1750Sensor_h

#include "MyMySensor.h"
#include <BH1750.h>

namespace mymysensors {


class BH1750Sensor: public MyMySensor
{
  BH1750 lightSensor_;
  MyValue<uint16_t> luminance_;
  void begin_() override {
    lightSensor_.begin(BH1750_ONE_TIME_HIGH_RES_MODE);
  }
  unsigned long preUpdate_() override {
    lightSensor_.configure(BH1750_ONE_TIME_HIGH_RES_MODE);
    return 120;
  }
  void update_() override {
    luminance_.update(lightSensor_.readLightLevel());
  }
public:
  BH1750Sensor(uint8_t sensorId, uint16_t treshold = 0)
    : luminance_(sensorId, V_LIGHT_LEVEL, S_LIGHT_LEVEL, treshold) {}
};

} // mymysensors

#endif //BH1750Sensor_h
