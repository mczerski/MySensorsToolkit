#include "utils.h"

extern bool isTransportReady();
extern void wait(const uint32_t);

namespace mys_toolkit {

void checkTransport()
{
  for (int i=0; i<20; i++) {
    if (isTransportReady())
      break;
    wait(500); // transport is not operational, allow the transport layer to fix this
  }
}

} //mys_toolkit
