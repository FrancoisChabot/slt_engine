#ifndef SLT_CONCUR_WORKER_H
#define SLT_CONCUR_WORKER_H

#include "slt/concur/blocking_queue.h"

#include <thread>
#include <functional>

namespace slt {
namespace concur {

class Worker {
public:
  using Task = std::function<void(void)>;
  Worker(BlockingQueue<Task>* src, std::string name);
  ~Worker();
  void main();

private:
  BlockingQueue<Task>* src_;
  std::string name_;
  std::thread thread_;
};

}  // namespace concur
}  // namespace slt

#endif
