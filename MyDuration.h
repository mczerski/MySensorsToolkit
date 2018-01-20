#ifndef MyDuration_h
#define MyDuration_h

#include "prescaler.h"
#include <limits.h>


namespace mymysensors {

class MyDuration
{
public:
  static void setClockPrescaler(uint8_t clockPrescaler) {
    setClockPrescaler(clockPrescaler);
  }
  typedef unsigned long duration_ms_t;
  explicit MyDuration(duration_ms_t duration) : duration_(rescaleDuration(duration)) {}
  explicit MyDuration() : duration_(millis()) {}
  duration_ms_t getMilis() const {
    return rescaleTime(duration_);
  }
  duration_ms_t get() const {
    return duration_;
  }
  bool operator<(const MyDuration &other){
    duration_ms_t difference = duration_ - other.duration_;
    return difference >= ULONG_MAX/2;
  }
  void operator+=(const MyDuration &other) {
    duration_ += other.duration_;
  }
  void operator*=(int factor) {
    duration_ *= factor;
  }
  MyDuration operator+(const MyDuration &other) {
    MyDuration result = *this;
    result += other;
    return result;
  }
private:
  duration_ms_t duration_;
};

} // mymysensors

#endif //MyDuration_h
