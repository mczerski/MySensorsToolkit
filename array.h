#ifndef array_h
#define array_h

namespace mymysensors {

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

} // mymysensors

#endif //array_h
