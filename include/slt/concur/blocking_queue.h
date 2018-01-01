#ifndef SLT_CONCUR_BLOCKING_QUEUE_H
#define SLT_CONCUR_BLOCKING_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace slt {
namespace concur {

// A simple mutli-threaded blocking queue.
template <typename T>
class BlockingQueue {
 public:
  // Block until something is available.
  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (queue_.empty()) {
      cond_.wait(lock);
    }
    auto item = std::move(queue_.front());
    queue_.pop();
    return item;
  }

  template <typename U>
  inline void push(U&& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.emplace(std::forward<U>(item));
    lock.unlock();
    cond_.notify_one();
  }

  inline void clear() {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_ = std::queue<T>();
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};
}  // namespace concur
}  // namespace slt

#endif
