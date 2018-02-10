#ifndef BME280Sensor_h
#define BME280Sensor_h

#include "MyMySensor.h"
#include <BME280I2C.h>

namespace mymysensors {

class BME280HumiditySensor: public MyMySensorBase<float>
{
  BME280I2C bmeSensor_;
  void begin_() override {
    if(!bmeSensor_.begin()){
      #ifdef MY_MY_DEBUG
      Serial.println("Could not find BME280 sensor!");
      #endif
    }
  }
  float getValue_() override {
    return bmeSensor_.hum();
  }
  unsigned long preUpdate_() override {
    bmeSensor_.setMode(1);
    return 120;
  }
public:
  BME280HumiditySensor(uint8_t sensorId, uint8_t type, uint8_t sensorType, float treshold = 0)
    : MyMySensorBase(sensorId, type, sensorType, treshold), bmeSensor_(1, 1, 1, 0) {}
};

} // mymysensors

#endif //BME280Sensor_h
