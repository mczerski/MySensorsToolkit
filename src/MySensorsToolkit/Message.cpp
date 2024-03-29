#include "Message.h"
#include <MySensorsToolkit/MySensors.h>

#ifdef MYS_TOOLKIT_DEBUG
extern HardwareSerial MYS_TOOLKIT_SERIAL;
#endif

namespace mys_toolkit {

bool Message::send_(Message &msg)
{
  if (::send(msg.msg_, false)) {
    msg.state_ = SENT;
    return true;
  }
  #ifdef MYS_TOOLKIT_DEBUG
  MYS_TOOLKIT_SERIAL.print("Message: failed to send ");
  MYS_TOOLKIT_SERIAL.print("t=");
  MYS_TOOLKIT_SERIAL.print(msg.getType());
  MYS_TOOLKIT_SERIAL.print(",c=");
  MYS_TOOLKIT_SERIAL.println(msg.getSensor());
  #endif
  return false;
}

Message::Message(uint8_t sensor, mysensors_data_t type)
  : msg_(sensor, type)
{
  if (messagesNum_ < MAX_MESSAGES) {
    messages_[messagesNum_++] = this;
  }
}

void Message::update()
{
  for (int i=0; i<messagesNum_; i++) {
    Message &msg = *messages_[i];
    if (msg.state_ == WAITING_TO_SEND) {
      send_(msg);
      return;
    }
  }
}

SendAllResult Message::sendAll()
{
  SendAllResult result{.success = 1, .somethingSent = 0};
  for (int i=0; i<messagesNum_; i++) {
    Message &msg = *messages_[i];
    if (msg.state_ == WAITING_TO_SEND) {
      if (send_(msg)) {
        result.somethingSent = 1;
      }
      else {
        result.success = 0;
      }
    }
  }
  return result;
}

uint8_t Message::getSensor() const
{
  return msg_.sensor;
}

uint8_t Message::getType() const
{
  return msg_.type;
}

Message* Message::messages_[];
int Message::messagesNum_ = 0;

} //mys_toolkit
