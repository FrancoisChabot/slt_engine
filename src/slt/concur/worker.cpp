#include "slt/concur/worker.h"
#include "slt/log.h"

namespace slt {

  namespace concur {

    Worker::Worker(BlockingQueue<Task>* src, std::string name)
      : src_(src)
      , name_(std::move(name))
      , thread_(&Worker::main, this) {
    }

    Worker::~Worker() {
      thread_.join();
    }

    void Worker::main() {
      slt::log->info("Starting worker thread: {}", name_);
      while (1) {
        auto op = src_->pop();
        if (!op) {
          break;
        }
        op();
      }
      slt::log->info("Worker thread finished: {}", name_);
    }
  }
}