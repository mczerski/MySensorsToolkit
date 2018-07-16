#pragma once

#include <stdint.h>
#include <core/MyMessage.h>

extern bool present(const uint8_t sensorId, const uint8_t sensorType, const char *description="",
                    const bool ack = false);
extern bool send(MyMessage &msg, const bool ack = false);
extern bool sendBatteryLevel(const uint8_t level, const bool ack = false);
extern void wait(const uint32_t waitingMS);
extern bool wait(const uint32_t waitingMS, const uint8_t cmd, const uint8_t msgtype);
extern int8_t sleep(const uint8_t interrupt, const uint8_t mode, const uint32_t sleepingMS = 0,
                    const bool smartSleep = false);
extern int8_t sleep(const uint8_t interrupt1, const uint8_t mode1, const uint8_t interrupt2,
                    const uint8_t mode2, const uint32_t sleepingMS = 0, const bool smartSleep = false);
extern bool isTransportReady();
extern void saveState(const uint8_t pos, const uint8_t value);
extern uint8_t loadState(const uint8_t pos);

