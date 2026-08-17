#include "KeyEvents.hpp"

namespace core {
  bool KeyEvents::enqueueEvent(int8_t event) {
    if (event == 0) return false;
    if (size == event_buffer.size()) return false;

    event_buffer[(begin + size) % event_buffer.size()] = event;
    ++size;
   
    return true;
  }

  int8_t KeyEvents::dequeueEvent() {
    if (size == 0) return 0;

    const int8_t retval = event_buffer[begin];
    event_buffer[begin] = 0;
    begin = (begin + 1) % event_buffer.size();
    --size;

    return retval;
  }

  void KeyEvents::clear() {
    *this = {};
  }
}
