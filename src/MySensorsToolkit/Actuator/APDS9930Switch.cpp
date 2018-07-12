#include "APDS9930Switch.h"

namespace mys_toolkit {

void MyAPDS9930::pcaSelect_(uint8_t i)
{
  if (apdsNum_ == 1)
    return;
  else {
    if (i > apdsNum_)
      return;
    Wire.beginTransmission(PCAADDR);
    Wire.write(4 + i);
    Wire.endTransmission();
  }
}

uint8_t MyAPDS9930::pcaGet_()
{
  if (apdsNum_ == 1)
    return digitalRead(intPin_) == LOW ? 1 : 0;
  else {
    Wire.requestFrom(PCAADDR, 1);
    return Wire.read() >> 4;
  }
}

void MyAPDS9930::init_(uint8_t i)
{
  pcaSelect_(i);
  bool status = apds_[i].init();
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print(F("APDS-9930 initializing sensor #"));
  Serial.println(i);
  if (status) {
    Serial.println(F("APDS-9930 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9930 init!"));
  }
  #endif
  status = apds_[i].enableProximitySensor(true);
  #ifdef MYS_TOOLKIT_DEBUG
  if (status) {
    Serial.println(F("Proximity sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }
  #endif
  status = apds_[i].setProximityGain(PGAIN_1X);
  #ifdef MYS_TOOLKIT_DEBUG
  if (!status) {
    Serial.println(F("Something went wrong trying to set PGAIN"));
  }
  #endif
  status = apds_[i].setProximityIntLowThreshold(PROX_INT_LOW);
  #ifdef MYS_TOOLKIT_DEBUG
  if (!status) {
    Serial.println(F("Error writing low threshold"));
  }
  #endif
  status = apds_[i].setProximityIntHighThreshold(PROX_INT_HIGH);
  #ifdef MYS_TOOLKIT_DEBUG
  if (!status) {
    Serial.println(F("Error writing high threshold"));
  }
  #endif
  (void)status;
}

bool MyAPDS9930::update_(uint8_t i)
{
  pcaSelect_(i);
  if (apds_[i].getProximityInt() == 0)
    return false;
  uint16_t proximity_data = 0;
  #ifdef MYS_TOOLKIT_DEBUG
  Serial.print("Reading sensor #");
  Serial.println(i);
  #endif
  if (!apds_[i].readProximity(proximity_data)) {
    #ifdef MYS_TOOLKIT_DEBUG
    Serial.println("Error reading proximity value");
    #endif
  } else {
    #ifdef MYS_TOOLKIT_DEBUG
    Serial.print("Proximity detected! Level: ");
    Serial.println(proximity_data);
    #endif
  }
  if (proximity_data < PROX_INT_HIGH) {
    if (!apds_[i].clearProximityInt()) {
      #ifdef MYS_TOOLKIT_DEBUG
      Serial.println("Error clearing interrupt");
      #endif
    }
  }
  return true;
}

MyAPDS9930::MyAPDS9930(uint8_t intPin, int apdsNum)
  : intPin_(intPin), apdsNum_(apdsNum), apdsInts_(0)
{
}

void MyAPDS9930::init()
{
  pinMode(intPin_, INPUT_PULLUP);
  for (uint8_t i=0; i<apdsNum_; i++)
    init_(i);
}

void MyAPDS9930::update()
{
  uint8_t pca = pcaGet_();
  apdsInts_ &= pca;
  if (pca) {
    for (uint8_t i=0; i<apdsNum_; i++) {
      if (pca & (1 << i)) {
        if (update_(i)) {
          apdsInts_ |= (1 << i);
        }
      }
    }
  }
}

bool MyAPDS9930::getInt(uint8_t i) const
{
  return apdsInts_ & (1 << i);
}

bool APDS9930Switch::doUpdate_()
{
  return myApds_.getInt(apdsNo_);
}

APDS9930Switch::APDS9930Switch(const MyAPDS9930 &myApds, uint8_t apdsNo)
  : Switch(false), myApds_(myApds), apdsNo_(apdsNo)
{
}

} //mys_toolkit
