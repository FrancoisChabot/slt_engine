#include "slt/concur/event_queue.h"

namespace slt {
namespace concur {
void EventQueue::queueEvent(Event evt) {
  std::lock_guard<std::mutex> lock(mutex_);
  queue_.push(move(evt));
  condition_.notify_one();
}

unsigned int EventQueue::executeAll() {
  // The predicate will get optimized away.
  return executeUntil([]() { return true; });
}
}  // namespace concur
}  // namespace slt