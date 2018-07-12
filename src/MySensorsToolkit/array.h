#ifndef array_h
#define array_h

#include <stddef.h>

namespace mys_toolkit {

template <typename T, size_t N>
struct array {
  T data[N];
  template<typename ...Ts>
  array(Ts && ...values)
    : data{static_cast<T>(values)...}
  {}
  T& operator[](size_t n) {
    return data[n];
  }
  const T& operator[](size_t n) const {
    return data[n];
  }
};

} //mys_toolkit

#endif //array_h
