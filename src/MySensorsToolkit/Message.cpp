#include "Message.h"
#include <MySensorsToolkit/MySensors.h>

namespace mys_toolkit {

void Message::send_(Message &msg)
{
  if (::send(msg.msg_, false)) {
    msg.state_ = SENT;
    return;
  }
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("Message: failed to send ");
  Serial.print("t=");
  Serial.print(msg.getType());
  Serial.print(",c=");
  Serial.println(msg.getSensor());
  #endif
}

Message::Message(uint8_t sensor, uint8_t type)
  : msg_(sensor, type)
{
  if (messagesNum_ < MAX_MESSAGES) {
    messages_[messagesNum_++] = this;
  }
}

void Message::setSent(const MyMessage& msg)
{
  for (int i=0; i<messagesNum_; i++) {
    if (messages_[i]->getSensor() == msg.sensor and messages_[i]->getType() == msg.type)
      messages_[i]->state_ = SENT;
  }
}

void Message::update()
{
  for (int i=0; i<messagesNum_; i++) {
    Message &msg = *messages_[i];
    if (msg.state_ == WAITING_FOR_ACK) {
      #ifdef MYS_TOOLKIT_DEBUG
      Serial.print("Message: resending ");
      Serial.print("t=");
      Serial.print(msg.getType());
      Serial.print(",c=");
      Serial.println(msg.getSensor());
      #endif
      send_(msg);
      if (msg.state_ == WAITING_FOR_ACK)
        return;
    }
  }
  for (int i=0; i<messagesNum_; i++) {
    Message &msg = *messages_[i];
    if (msg.state_ == WAITING_TO_SEND) {
      msg.state_ = WAITING_FOR_ACK;
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
      if (::send(msg.msg_, false)) {
        msg.state_ = SENT;
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
