#include <MySensorsToolkit/MySensors.h>
#include <MySensorsToolkit/utils.h>

namespace mys_toolkit {

class ParameterBase {
protected:
  MyMessage msg_;
  uint8_t sensorId_;
  uint8_t sensorType_;
  uint8_t parameterPoition_;
  static uint8_t parametersCount_;
  static constexpr uint8_t MAX_PARAMETERS = 10;
  static ParameterBase* parameters_[MAX_PARAMETERS];
  static uint8_t globalParameterPosition_;

  void present_();
  virtual void set_(const MyMessage &message) = 0;

public:
  ParameterBase(uint8_t sensorId, uint8_t type, uint8_t sensorType);
  static void present();
  static void receive(const MyMessage &message);
};

template <typename ValueType>
class Parameter : public ParameterBase {
  static constexpr ValueType erasedValue = static_cast<ValueType>(0xffffffff);

  void save_(ValueType value) {
    uint32_t tmp = value;
    for (uint8_t i=0; i<sizeof(ValueType); i++) {
      saveState(parameterPoition_ + i, (tmp >> i) & 0xff);
    }
  }

  ValueType load_() {
    uint32_t tmp = 0;
    for (uint8_t i=0; i<sizeof(ValueType); i++) {
      tmp |= (static_cast<uint32_t>(loadState(parameterPoition_ + i)) << i);
    }
    return ValueType(tmp);
  }

  void set_(const MyMessage &message) override {
    auto value = getMessageValue<ValueType>(message);
    if (value == erasedValue) 
      return;
    save_(value);
    setMessageValue(msg_, value);
  }
public:
  Parameter(uint8_t sensorId, uint8_t type, uint8_t sensorType, ValueType init = 0)
    : ParameterBase(sensorId, type, sensorType)
  {
    static_assert(sizeof(ValueType) <= sizeof(uint32_t), "MyParameter may be instantiated only with types not grater than 32 bits");
    globalParameterPosition_ += sizeof(ValueType);
    ValueType eeprom = load_();
    if (eeprom == erasedValue)
      save_(init);
    else
      init = eeprom;
    setMessageValue(msg_, init);
  }
  ValueType get() {
    return getMessageValue<ValueType>(msg_);
  }
};

} //mys_toolkit
