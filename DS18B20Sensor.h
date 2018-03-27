#ifndef DS18B20Sensor_h
#define DS18B20Sensor_h

#include "MyMySensor.h"
#include <DallasTemperature.h>
#include <OneWire.h>

namespace mymysensors {


class DS18B20Sensor: public MyMySensor
{
  OneWire oneWire_;
  DallasTemperature sensor_;
  uint8_t powerPin_;
  MyValue<float> temperature_;
  void power(uint8_t value) {
    if (powerPin_ != uint8_t(-1))
      digitalWrite(powerPin_, value);
  }
  void begin_() override {
    if (powerPin_ != uint8_t(-1))
      pinMode(powerPin_, OUTPUT);
    power(HIGH);
    sensor_.begin();
    sensor_.setWaitForConversion(false);
#ifdef MY_MY_DEBUG
    auto devCount = sensor_.getDeviceCount();
    Serial.print("DS18B20: Found ");
    Serial.print(devCount);
    Serial.println(" devices");
    auto isParasite = sensor_.isParasitePowerMode();
    Serial.print("DS18B20: parasite: ");
    Serial.println(isParasite);
#endif
  }
  unsigned long preUpdate_() override {
    power(HIGH);
    sensor_.requestTemperatures();
    return sensor_.millisToWaitForConversion(sensor_.getResolution());
  }
  void update_() override {
    auto temp = sensor_.getTempCByIndex(0);
#ifdef MY_MY_DEBUG
    auto devCount = sensor_.getDeviceCount();
    Serial.print("DS18B20: temp: ");
    Serial.println(temp);
#endif
    temperature_.update(temp);
    power(LOW);
  }
public:
  DS18B20Sensor(uint8_t tempSensorId, uint8_t dataPin, float tempTreshold = 0, uint8_t powerPin = -1)
    : oneWire_(dataPin),
      sensor_(&oneWire_),
      powerPin_(powerPin),
      temperature_(tempSensorId, V_TEMP, S_TEMP, tempTreshold) {}
};

} // mymysensors

#endif //DS18B20Sensor_h
