#ifndef SceneController_h
#define SceneController_h

#include "ActuatorBase.h"
#include "Message.h"
#include "Switch.h"
#include "Duration.h"

namespace mys_toolkit {

class SceneController : public ActuatorBase
{
  enum State {
    WAITING_FOR_RISING,
    WAITING_FOR_SCENE,
    WAITING_FOR_FALLING
  };
  State state_;
  bool prevSwState_;
  Duration lastSwRiseTime_;
  Switch &sw_;
  Message sceneMsg_;
  bool enableShortPress_;
  bool isRising_(bool swState);
  bool isHeldLongEnough_(bool swState);
  bool isFalling(bool swState);
  void sendScene_(uint8_t scene);
  void update_() override;

public:
  SceneController(uint8_t sensorId, Switch &sw, bool enableShortPress=true);

};

} //mys_toolkit

#endif //SceneController_h

