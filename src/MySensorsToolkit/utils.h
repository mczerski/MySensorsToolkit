#ifndef utils_h
#define utils_h

#include <core/MyMessage.h>

namespace mys_toolkit {

template <typename ValueType>
inline void setMessageValue(MyMessage &msg, ValueType value) {
  msg.set(value);
}

template <>
inline void setMessageValue(MyMessage &msg, float value)
{
  msg.set(value, 1);
}

void checkTransport();
bool sendAndWait(MyMessage &msg, uint32_t wait_ms);

void logMsg(const char * const text);

} //mys_toolkit

#endif //SensorBases_h
