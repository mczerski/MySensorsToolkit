#ifndef BH1750Sensor_h
#define BH1750Sensor_h

#include "SensorBase.h"
#include <BH1750.h>

namespace mys_toolkit {


class BH1750Sensor: public SensorBase
{
  BH1750 lightSensor_;
  SensorValue<uint16_t> luminance_;
  void begin_() override {
    lightSensor_.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  }
  unsigned long preUpdate_() override {
    lightSensor_.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
    return 120;
  }
  unsigned long update_() override {
    luminance_.update(lightSensor_.readLightLevel());
    return SLEEP_TIME;
  }
public:
  BH1750Sensor(uint8_t sensorId, uint16_t treshold = 0)
    : luminance_(sensorId, V_LIGHT_LEVEL, S_LIGHT_LEVEL, treshold) {}
};

} //mys_toolkit

#endif //BH1750Sensor_h
