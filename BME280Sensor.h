#ifndef BME280Sensor_h
#define BME280Sensor_h

#include "SensorBase.h"
#include <BME280I2C.h>

namespace mys_toolkit {


class BME280Sensor: public SensorBase
{
  BME280I2C bmeSensor_;
  SensorValue<float> humidity_;
  SensorValue<float> temperature_;
  void begin_() override {
    if(!bmeSensor_.begin()){
      #ifdef MY_MY_DEBUG
      Serial.println("Could not find BME280 sensor!");
      #endif
    }
  }
  unsigned long preUpdate_() override {
    bmeSensor_.setMode(1);
    return 120;
  }
  unsigned long update_() override {
    humidity_.update(bmeSensor_.hum());
    temperature_.update(bmeSensor_.temp());
    return SLEEP_TIME;
  }
public:
  BME280Sensor(uint8_t humSensorId, uint8_t tempSensorId, float humTreshold = 0, float tempTreshold = 0)
    : bmeSensor_(1, 1, 1, 0),
      humidity_(humSensorId, V_HUM, S_HUM, humTreshold),
      temperature_(tempSensorId, V_TEMP, S_TEMP, tempTreshold) {}
};

} //mys_toolkit

#endif //BME280Sensor_h
