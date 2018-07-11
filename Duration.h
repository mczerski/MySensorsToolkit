#ifndef Duration_h
#define Duration_h

#include "prescaler.h"
#include <limits.h>


namespace mys_toolkit {

class Duration
{
public:
  static void setClockPrescaler(uint8_t clockPrescaler) {
    ::setClockPrescaler(clockPrescaler);
  }
  typedef unsigned long duration_ms_t;
  explicit Duration(duration_ms_t duration) : duration_(rescaleDuration(duration)) {}
  explicit Duration() : duration_(millis()) {}
  duration_ms_t getMilis() const {
    return rescaleTime(duration_);
  }
  duration_ms_t get() const {
    return duration_;
  }
  bool operator<(const Duration &other){
    duration_ms_t difference = duration_ - other.duration_;
    return difference >= ULONG_MAX/2;
  }
  void operator+=(const Duration &other) {
    duration_ += other.duration_;
  }
  void operator*=(int factor) {
    duration_ *= factor;
  }
  Duration operator+(const Duration &other) {
    Duration result = *this;
    result += other;
    return result;
  }
private:
  duration_ms_t duration_;
};

} //mys_toolkit

#endif //Duration_h
