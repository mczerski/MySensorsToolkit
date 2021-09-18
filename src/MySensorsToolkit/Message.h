#pragma once

#include <MySensorsToolkit/utils.h>

#include <core/MyMessage.h>

namespace mys_toolkit {

enum MessageState {
  SENT,
  WAITING_TO_SEND
};

struct SendAllResult {
  uint8_t success : 1;
  uint8_t somethingSent : 1;
};

class Message
{
  static constexpr int MAX_MESSAGES = 10;
  MyMessage msg_;
  MessageState state_ = SENT;
  static Message* messages_[MAX_MESSAGES];
  static int messagesNum_;
  static bool send_(Message &msg);
public:
  Message(uint8_t sensor, mysensors_data_t type);
  static void update();
  static SendAllResult sendAll();
  template <typename ValueType>
  void send(ValueType value)
  {
    state_ = WAITING_TO_SEND;
    setMessageValue(msg_, value);
  }
  uint8_t getSensor() const;
  uint8_t getType() const;
};

} //mys_toolkit

