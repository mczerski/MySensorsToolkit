#ifndef Message_h
#define Message_h

#include "utils.h"

#include <MySensors.h>

namespace mys_toolkit {

static const int MAX_MY_MY_MESSAGES = 10;

enum MessageState {
  SENT,
  WAITING_FOR_ACK,
  WAITING_TO_SEND
};

class Message
{
  MyMessage msg_;
  unsigned long sendTime_ = 0;
  MessageState state_ = SENT;
  static Message* messages_[MAX_MY_MY_MESSAGES];
  static int messagesNum_;
  static void send_(Message &msg) {
    msg.sendTime_ = millis();
    if (::send(msg.msg_, true))
      return;
	#ifdef MY_MY_DEBUG
	Serial.print("Message: failed to send ");
	Serial.print("t=");
	Serial.print(msg.getMyMessage().type);
	Serial.print(",c=");
	Serial.println(msg.getMyMessage().sensor);
	#endif
  }
public:
  Message(uint8_t sensor, uint8_t type) : msg_(sensor, type)
  {
    if (messagesNum_ < MAX_MY_MY_MESSAGES) {
      messages_[messagesNum_++] = this;
    }
  }
  static void setSent(const MyMessage& msg)
  {
    for (int i=0; i<messagesNum_; i++) {
      const MyMessage& myMsg = messages_[i]->getMyMessage();
      if (myMsg.sensor == msg.sensor and myMsg.type == msg.type)
        messages_[i]->state_ = SENT;
    }
  }
  static void update()
  {
    for (int i=0; i<messagesNum_; i++) {
      Message &msg = *messages_[i];
      if (msg.state_ == WAITING_FOR_ACK) {
        if (millis() - msg.sendTime_ > 2000) {
          #ifdef MY_MY_DEBUG
          Serial.print("Message: resending ");
          Serial.print("t=");
          Serial.print(msg.getMyMessage().type);
          Serial.print(",c=");
          Serial.println(msg.getMyMessage().sensor);
          #endif
          send_(msg);
        }
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
  template <typename ValueType>
  void send(ValueType value)
  {
    state_ = WAITING_TO_SEND;
    setMessageValue(msg_, value);
  }
  const MyMessage& getMyMessage() const {
    return msg_;
  }
};

Message* Message::messages_[];
int Message::messagesNum_ = 0;

} //mys_toolkit

#endif //Message_h