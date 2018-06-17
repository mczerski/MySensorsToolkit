#ifndef MyMySensorsBase_h
#define MyMySensorsBase_h

#include "MyDuration.h"
#include "MyMySensors.h"

#include <SoftTimer.h>

namespace mymysensors {

static const int MAX_MY_MY_MESSAGES = 10;

enum MyMyMessageState {
  SENT,
  WAITING_FOR_ACK,
  WAITING_TO_SEND
};

class MyMyMessage
{
  MyMessage msg_;
  unsigned long sendTime_ = 0;
  MyMyMessageState state_ = SENT;
  static MyMyMessage* messages_[MAX_MY_MY_MESSAGES];
  static int messagesNum_;
  static void send_(MyMyMessage &msg) {
    msg.sendTime_ = millis();
    if (::send(msg.msg_, true))
      return;
	#ifdef MY_MY_DEBUG
	Serial.print("MyMyMessage: failed to send ");
	Serial.print("t=");
	Serial.print(msg.getMyMessage().type);
	Serial.print(",c=");
	Serial.println(msg.getMyMessage().sensor);
	#endif
  }
public:
  MyMyMessage(uint8_t sensor, uint8_t type) : msg_(sensor, type)
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
      MyMyMessage &msg = *messages_[i];
      if (msg.state_ == WAITING_FOR_ACK) {
        if (millis() - msg.sendTime_ > 2000) {
          #ifdef MY_MY_DEBUG
          Serial.print("MyMyMessage: resending ");
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
      MyMyMessage &msg = *messages_[i];
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
    setMessageValue_(msg_, value);
  }
  const MyMessage& getMyMessage() const {
    return msg_;
  }
};

MyMyMessage* MyMyMessage::messages_[];
int MyMyMessage::messagesNum_ = 0;

class MyMySensorsBase
{
  virtual void begin_() {}
  virtual void update_() {}
  virtual void firstUpdate_() {}
  virtual void receive_(const MyMessage &) {}
  uint8_t sensorId_;
  uint8_t sensorType_;
  static constexpr uint8_t MAX_SENSORS = 10;
  static uint8_t sensorsCount_;
  static MyMySensorsBase* sensors_[MAX_SENSORS];
  static bool loopCalled_;
  static SoftTimer timer_;

public:
  MyMySensorsBase(uint8_t sensorId, uint8_t sensorType)
    : sensorId_(sensorId), sensorType_(sensorType)
  {
    if (sensorsCount_ < MAX_SENSORS)
      sensors_[sensorsCount_++] = this;
  }
  static void begin() {
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->begin_();
  }
  static void present() {
    for (size_t i=0; i<sensorsCount_; i++)
      ::present(sensors_[i]->sensorId_, sensors_[i]->sensorType_);
  }
  static void update() {
    timer_.update();
    MyMyMessage::update();
    for (size_t i=0; i<sensorsCount_; i++)
      sensors_[i]->update_();
    if (not loopCalled_) {
      checkTransport();
      for (size_t i=0; i<sensorsCount_; i++)
        sensors_[i]->firstUpdate_();
      loopCalled_ = true;
    }
  }
  static void receive(const MyMessage &message) {
    if (message.isAck()) {
      MyMyMessage::setSent(message);
      return;
    }
    for (size_t i=0; i<sensorsCount_; i++)
      if (sensors_[i]->sensorId_ == message.sensor)
        sensors_[i]->receive_(message);
  }
  static int16_t addEvent(EventBase* evt) {
    return timer_.addEvent(evt);
  }
};

uint8_t MyMySensorsBase::sensorsCount_ = 0;
MyMySensorsBase * MyMySensorsBase::sensors_[];
bool MyMySensorsBase::loopCalled_ = false;
SoftTimer MyMySensorsBase::timer_;

} // mymysensors

#endif //MyMySensorsBase_h
