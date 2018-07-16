#ifndef utils_h
#define utils_h

#include <core/MyMessage.h>

namespace mys_toolkit {

template <typename ValueType>
inline void setMessageValue(MyMessage &msg, ValueType value) {
  msg.set(value);
}

template <>
inline void setMessageValue<float>(MyMessage &msg, float value)
{
  msg.set(value, 1);
}

template<typename ValueType>
ValueType getMessageValue(const MyMessage &msg) {
  return ValueType();
}

template <>
inline bool getMessageValue<bool>(const MyMessage &msg) {
  return msg.getBool();
}

template <>
inline uint8_t getMessageValue<uint8_t>(const MyMessage &msg) {
  return msg.getByte();
}

template <>
inline float getMessageValue<float>(const MyMessage &msg) {
  return msg.getFloat();
}

template <>
inline int16_t getMessageValue<int16_t>(const MyMessage &msg) {
  return msg.getInt();
}

template <>
inline uint16_t getMessageValue<uint16_t>(const MyMessage &msg) {
  return msg.getUInt();
}

template <>
inline int32_t getMessageValue<int32_t>(const MyMessage &msg) {
  return msg.getLong();
}

template <>
inline uint32_t getMessageValue<uint32_t>(const MyMessage &msg) {
  return msg.getULong();
}

void checkTransport();
bool sendAndWait(MyMessage &msg, uint32_t wait_ms);

void logMsg(const char * const text);

} //mys_toolkit

#endif //SensorBases_h
