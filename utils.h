#ifndef utils_h
#define utils_h

#include <MySensors.h>

namespace mys_toolkit {

template <typename ValueType>
void setMessageValue(MyMessage &msg, ValueType value) {
  msg.set(value);
}

template <>
void setMessageValue(MyMessage &msg, float value) {
  msg.set(value, 1);
}

void checkTransport() {
  for (int i=0; i<20; i++) {
    if (isTransportReady())
      break;
    wait(500); // transport is not operational, allow the transport layer to fix this
  }
}

} //mys_toolkit

#endif //SensorBases_h
