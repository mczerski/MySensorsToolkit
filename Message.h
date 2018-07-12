#ifndef Message_h
#define Message_h

#include "utils.h"

#include <core/MyMessage.h>

namespace mys_toolkit {

enum MessageState {
  SENT,
  WAITING_FOR_ACK,
  WAITING_TO_SEND
};

class Message
{
  static constexpr int MAX_MESSAGES = 10;
  MyMessage msg_;
  unsigned long sendTime_ = 0;
  MessageState state_ = SENT;
  static Message* messages_[MAX_MESSAGES];
  static int messagesNum_;
  static void send_(Message &msg);
public:
  Message(uint8_t sensor, uint8_t type);
  static void setSent(const MyMessage& msg);
  static void update();
  template <typename ValueType>
  void send(ValueType value)
  {
    state_ = WAITING_TO_SEND;
    setMessageValue(msg_, value);
  }
  const MyMessage& getMyMessage();
};

} //mys_toolkit

#endif //Message_h