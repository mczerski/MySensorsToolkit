#include "Duration.h"

#include <limits.h>

namespace mys_toolkit {

void Duration::setClockPrescaler(uint8_t clockPrescaler)
{
  ::setClockPrescaler(clockPrescaler);
}

Duration::Duration(Duration::duration_ms_t duration)
  : duration_(rescaleDuration(duration)) 
{
}

Duration::Duration()
  : duration_(millis()) 
{
}

Duration::duration_ms_t Duration::getMilis() const
{
  return rescaleTime(duration_);
}

Duration::duration_ms_t Duration::get() const
{
  return duration_;
}

bool Duration::operator<(const Duration &other)
{
  Duration::duration_ms_t difference = duration_ - other.duration_;
  return difference >= ULONG_MAX/2;
}

bool Duration::operator<=(const Duration &other)
{
  return this->duration_ == other.duration_ or *this < other;
}

void Duration::operator+=(const Duration &other)
{
  duration_ += other.duration_;
}

void Duration::operator*=(int factor)
{
  duration_ *= factor;
}

Duration Duration::operator+(const Duration &other)
{
  Duration result = *this;
  result += other;
  return result;
}

} //mys_toolkit
