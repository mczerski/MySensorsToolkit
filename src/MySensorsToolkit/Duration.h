#pragma once

#include "prescaler.h"

namespace mys_toolkit {

class Duration
{
public:
  static void setClockPrescaler(uint8_t clockPrescaler);
  typedef unsigned long duration_ms_t;
  explicit Duration(duration_ms_t duration);
  explicit Duration();
  duration_ms_t getMilis() const;
  duration_ms_t get() const;
  bool operator<(const Duration &other);
  bool operator<=(const Duration &other);
  void operator+=(const Duration &other);
  void operator*=(int factor);
  Duration operator+(const Duration &other);
private:
  duration_ms_t duration_;
};

} //mys_toolkit

