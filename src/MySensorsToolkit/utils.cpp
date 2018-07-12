#include "MySensors.h"
#include "utils.h"

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
