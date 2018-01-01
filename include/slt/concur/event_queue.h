#ifndef SLT_CONCUR_EVENT_QUEUE_H
#define SLT_CONCUR_EVENT_QUEUE_H

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

namespace slt {
namespace concur {

// This class holds a FIFO queue of handlers to execute. Events can be
// afely queued from any thread.
class EventQueue {
 public:
  using Event = std::function<void(void)>;

  // Queues a new event.
  void queueEvent(Event);

  // Executes all queued events.
  // If there is no event in the queue, this will return immediately.
  //
  // Returns:
  //   the number of consumed events.
  unsigned int executeAll();

  // Executes all queued events until predicate returns true. By default, the
  // predicate is checked only when the queue is fully empty.
  //
  // Returns:
  //   the number of consumed events.
  //
  // Args:
  //   pred: The predicate to check to determine if we should abort
  //   batch_size: (optional) Check the predicate after this many events have
  //               been processed, even if there are events left in the queue.
  //               The predicate will always be checked when the queue is empty.
  template <typename PRED_T>
  inline unsigned int executeUntil(PRED_T pred, unsigned int batch_size = 0) {
    unsigned int executed_event_count = 0;
    unsigned int current_batch_count = 0;
    while (true) {
      std::unique_lock<std::mutex> lock(mutex_);
      if (queue_.empty()) {
        // Always check the predicate when we hit an empty queue.
        if (pred()) {
          break;
        } else {
          // The queue is empty and the predicate returned false, we can just
          // block until something goes in the queue.
          condition_.wait(lock);
          continue;
        }
      }

      // The order of events is important here:
      // Dequeue -> Unlock -> Invoke
      // This is because an event can enqueue a new event.
      Event cb = move(queue_.front());
      queue_.pop();
      lock.unlock();
      cb();

      ++executed_event_count;

      // Check to see if it's time to check the predicate.
      ++current_batch_count;
      if (batch_size != 0 && current_batch_count >= batch_size) {
        if (pred()) {
          break;
        } else {
          current_batch_count = 0;
        }
      }
    }

    return executed_event_count;
  }

 private:
  std::queue<Event> queue_;
  std::mutex mutex_;
  std::condition_variable condition_;
};
}  // namespace concur
}  // namespace slt

#endif
