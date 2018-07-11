#include "utils.h"

extern bool isTransportReady();
extern void wait(const uint32_t);
extern bool wait(const uint32_t waitingMS, const uint8_t cmd, const uint8_t msgtype);
extern bool send(MyMessage&, const bool);

namespace mys_toolkit {

void checkTransport()
{
  for (int i=0; i<20; i++) {
    if (isTransportReady())
      break;
    wait(500); // transport is not operational, allow the transport layer to fix this
  }
}

bool sendAndWait(MyMessage &msg, uint32_t wait_ms)
{
  return send(msg, true) and wait(wait_ms, C_SET, msg.type);
}

} //mys_toolkit
