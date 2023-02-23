#pragma once

#include "ActuatorBase.h"
#include <MySensorsToolkit/Message.h>
#include <MySensorsToolkit/Duration.h>

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

class RequestableValueBase : public EventBase, public ActuatorBase
{
  Duration interval_;
  void scheduleEvent(boolean (*cb)(EventBase*), Duration delayMs);
  static boolean readValue_(EventBase* event);
  static boolean startMeasurement_(EventBase* event);
  void begin_() override;
  virtual void begin2_();
  virtual void updateValueCb_() = 0;
  virtual Duration startMeasurementCb_() = 0;

public:
  RequestableValueBase(uint8_t sensorId, mysensors_sensor_t sensorType, Duration interval);
};

namespace std14
{
	template<typename T, T... Ints>
	struct integer_sequence
	{
		typedef T value_type;
		static constexpr std::size_t size() { return sizeof...(Ints); }
	};
	
	template<std::size_t... Ints>
	using index_sequence = integer_sequence<std::size_t, Ints...>;
	
	template<typename T, std::size_t N, T... Is>
	struct make_integer_sequence : make_integer_sequence<T, N-1, N-1, Is...> {};
	
	template<typename T, T... Is>
	struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...> {};
	
	template<std::size_t N>
	using make_index_sequence = make_integer_sequence<std::size_t, N>;
	
	template<typename... T>
	using index_sequence_for = make_index_sequence<sizeof...(T)>;
}

template <typename... ValueTypes>
class RequestableValue : public RequestableValueBase
{
public:
  static constexpr size_t VALUES_SIZE = sizeof...(ValueTypes);
  using Values = std::tuple<ValueTypes...>;
  using MessageArray = std::array<Message, VALUES_SIZE>;
  using TypeArray = std::array<mysensors_data_t,  VALUES_SIZE>;
  using ChangeArray = std::array<float, VALUES_SIZE>;
  RequestableValue(uint8_t sensorId, const TypeArray &types, mysensors_sensor_t sensorType, Duration interval, const ChangeArray &changeTresholds = {0})
    : RequestableValueBase(sensorId, sensorType, interval),
      msgs_(make_msg_array_(sensorId, types, std14::make_index_sequence<VALUES_SIZE>{})),
      changeTresholds_(changeTresholds)
  {}

private:
  template <size_t... Idxs>
  static MessageArray make_msg_array_(
    uint8_t sensorId,
    const TypeArray &types,
    std14::index_sequence<Idxs...> /*unused*/
  ) {
      return {Message(sensorId, types[Idxs])...};
  }
  MessageArray msgs_;
  Values values_;
  ChangeArray changeTresholds_;
  template <size_t Idx>
  bool doHandleReq_(const MyMessage &message) {
    if (message.type == msgs_[Idx].getType()) {
      msgs_[Idx].send(std::get<Idx>(values_));
      return true;
    }
    return false;
  }
  template <size_t Idx>
  bool handleReq_(const MyMessage &message, typename std::enable_if<Idx != 0, bool>::type = true) {
    if (handleReq_<Idx-1>(message)) {
        return true;
    }
    return doHandleReq_<Idx>(message);
  }
  template <size_t Idx>
  bool handleReq_(const MyMessage &message, typename std::enable_if<Idx == 0, bool>::type = true) {
    return doHandleReq_<Idx>(message);
  }
  void receive_(const MyMessage &message) override
  {
    if (mGetCommand(message) == C_REQ) {
      handleReq_<VALUES_SIZE-1>(message);
    }
    else {
      receive2_(message);
    }
  }
  template <size_t Idx>
  void doSendOnChange_(const Values &values) {
    #ifdef MYS_TOOLKIT_DEBUG
    for (size_t i=0; i<VALUES_SIZE; i++) {
      MYS_TOOLKIT_SERIAL.print("readValue(%d): ", i);
      MYS_TOOLKIT_SERIAL.println(std::get<Idx>(values));
    }
    #endif
    float change = 1.0 * std::abs((std::get<Idx>(values_) - std::get<Idx>(values)) / std::get<Idx>(values_));
    if (changeTresholds_[Idx] > 0 and change >= changeTresholds_[Idx]) {
      msgs_[Idx].send(std::get<Idx>(values));
    }
  }
  template <size_t Idx>
  void sendOnChange_(const Values &values, typename std::enable_if<Idx != 0, bool>::type = true) {
    sendOnChange_<Idx-1>(values);
    doSendOnChange_<Idx>(values);
  }
  template <size_t Idx>
  void sendOnChange_(const Values &values, typename std::enable_if<Idx == 0, bool>::type = true) {
    doSendOnChange_<Idx>(values);
  }
  void updateValueCb_() override {
    auto values = readValuesCb_();
    sendOnChange_<VALUES_SIZE-1>(values);
    values_ = values;
  }
  virtual Values readValuesCb_() = 0;
  virtual void receive2_(const MyMessage &message) {};
};

} //mys_toolkit

