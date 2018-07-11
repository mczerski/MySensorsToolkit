#ifndef Switch_h
#define Switch_h

namespace mys_toolkit {

class Switch {
  uint8_t activeLow_;
  virtual bool doUpdate_() = 0;
public:
  Switch(bool activeLow = false) : activeLow_(activeLow) {}
  bool update() {
    bool state = doUpdate_();
    return activeLow_ ? !state : state;
  }
};

} //mys_toolkit

#endif //Switch_h
