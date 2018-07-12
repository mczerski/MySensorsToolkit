#include "SceneController.h"

namespace mys_toolkit {

bool SceneController::isRising_(bool swState)
{
  return swState == true and prevSwState_ == false;
}

bool SceneController::isHeldLongEnough_(bool swState)
{
  return swState == true and prevSwState_ == true and lastSwRiseTime_ + Duration(1000) < Duration();
}

bool SceneController::isFalling(bool swState)
{
  return swState == false and prevSwState_ == true;
}

void SceneController::sendScene_(uint8_t scene)
{
  sceneMsg_.send(scene);
  #ifdef MY_MY_DEBUG
  Serial.print("sendScene_ ");
  Serial.print(scene);
  Serial.print(" for child id ");
  Serial.println(sceneMsg_.getMyMessage().sensor);
  #endif
}

void SceneController::update_()
{
  bool currSwState = sw_.update();
  if (isRising_(currSwState)) {
    lastSwRiseTime_ = Duration();
    state_ = WAITING_FOR_SCENE;
  }
  else if (state_ == WAITING_FOR_SCENE and isHeldLongEnough_(currSwState)) {
    state_ = WAITING_FOR_FALLING;
    sendScene_(1);
  }
  else if (isFalling(currSwState)) {
    if (state_ == WAITING_FOR_SCENE) {
      if (enableShortPress_) {
        sendScene_(0);
      }
    }
    state_ = WAITING_FOR_RISING;
  }
  prevSwState_ = currSwState;
}

SceneController::SceneController(uint8_t sensorId, Switch &sw, bool enableShortPress=true)
  : ActuatorBase(sensorId, S_SCENE_CONTROLLER),
    state_(WAITING_FOR_RISING),
    prevSwState_(false),
    sw_(sw),
    sceneMsg_(sensorId, V_SCENE_ON),
    enableShortPress_(enableShortPress)
{
}

} //mys_toolkit
