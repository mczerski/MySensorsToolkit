#ifndef APDS9930Switch_h
#define APDS9930Switch_h

#include "Switch.h"

#include <APDS9930.h>
#include <Wire.h>

namespace mys_toolkit {

class MyAPDS9930 {
  uint8_t intPin_;
  APDS9930 apds_[APDS9930_NUM];
  uint8_t apdsInts_;
  static const int PCAADDR = 0x70;
  static const int PROX_INT_HIGH = 900; // Proximity level for interrupt
  static const int PROX_INT_LOW = 0;  // No far interrupt

  void pcaSelect_(uint8_t i) {
    #if APDS9930_NUM == 1
    (void)i;
    return;
    #else
    if (i > 3) return;
    Wire.beginTransmission(PCAADDR);
    Wire.write(4 + i);
    Wire.endTransmission();
    #endif
  }
  uint8_t pcaGet_() {
    #if APDS9930_NUM == 1
    return digitalRead(intPin_) == LOW ? 1 : 0;
    #else
    Wire.requestFrom(PCAADDR, 1);
    return Wire.read() >> 4;
    #endif
  }
  void init_(uint8_t i) {
    pcaSelect_(i);
    bool status = apds_[i].init();
    #ifdef MY_MY_DEBUG
    Serial.print(F("APDS-9930 initializing sensor #"));
    Serial.println(i);
    if (status) {
      Serial.println(F("APDS-9930 initialization complete"));
    } else {
      Serial.println(F("Something went wrong during APDS-9930 init!"));
    }
    #endif
    status = apds_[i].enableProximitySensor(true);
    #ifdef MY_MY_DEBUG
    if (status) {
      Serial.println(F("Proximity sensor is now running"));
    } else {
      Serial.println(F("Something went wrong during sensor init!"));
    }
    #endif
    status = apds_[i].setProximityGain(PGAIN_1X);
    #ifdef MY_MY_DEBUG
    if (!status) {
      Serial.println(F("Something went wrong trying to set PGAIN"));
    }
    #endif
    status = apds_[i].setProximityIntLowThreshold(PROX_INT_LOW);
    #ifdef MY_MY_DEBUG
    if (!status) {
      Serial.println(F("Error writing low threshold"));
    }
    #endif
    status = apds_[i].setProximityIntHighThreshold(PROX_INT_HIGH);
    #ifdef MY_MY_DEBUG
    if (!status) {
      Serial.println(F("Error writing high threshold"));
    }
    #endif
    (void)status;
  }
  bool update_(uint8_t i) {
    pcaSelect_(i);
    if (apds_[i].getProximityInt() == 0)
      return false;
    uint16_t proximity_data = 0;
    #ifdef MY_MY_DEBUG
    Serial.print("Reading sensor #");
    Serial.println(i);
    #endif
    if (!apds_[i].readProximity(proximity_data)) {
      #ifdef MY_MY_DEBUG
      Serial.println("Error reading proximity value");
      #endif
    } else {
      #ifdef MY_MY_DEBUG
      Serial.print("Proximity detected! Level: ");
      Serial.println(proximity_data);
      #endif
    }
    if (proximity_data < PROX_INT_HIGH) {
      if (!apds_[i].clearProximityInt()) {
        #ifdef MY_MY_DEBUG
        Serial.println("Error clearing interrupt");
        #endif
      }
    }
    return true;
  }

public:
  MyAPDS9930(uint8_t intPin) : intPin_(intPin), apdsInts_(0) {}
  void init() {
    pinMode(intPin_, INPUT_PULLUP);
    for (uint8_t i=0; i<APDS9930_NUM; i++)
      init_(i);
  }
  void update() {
    uint8_t pca = pcaGet_();
    apdsInts_ &= pca;
    if (pca) {
      for (uint8_t i=0; i<APDS9930_NUM; i++) {
        if (pca & (1 << i)) {
          if (update_(i)) {
            apdsInts_ |= (1 << i);
          }
        }
      }
    }
  }
  bool getInt(uint8_t i) const {
    return apdsInts_ & (1 << i);
  }
};

class APDS9930Switch : public Switch {
  const MyAPDS9930 &myApds_;
  uint8_t apdsNo_;
  bool doUpdate_() override {
    return myApds_.getInt(apdsNo_);
  }
public:
  APDS9930Switch(const MyAPDS9930 &myApds, uint8_t apdsNo) : Switch(false), myApds_(myApds), apdsNo_(apdsNo) {}
};

} //mys_toolkit

#endif //APDS9930Switch_h
