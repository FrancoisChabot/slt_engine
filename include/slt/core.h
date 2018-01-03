#ifndef SLT_CORE_CORE_H
#define SLT_CORE_CORE_H

#include <string_view>
#include <vector>

#include "slt/settings.h"
#include "slt/concur/worker.h"

namespace slt {

// There can never be more that one slt core active at any point in time,
// accessible through Core::instance. Furthermore, many things assume the
// core instance to be set, so setting up a core is generally the first thing
// done at the top of main().

// Core provides 3 functionalities:
// 1. Initialize settings.
//     - Settings are reset to default values on destruction, to facilitate
//       unit testing.
// 2. Initializes logging.
// 3. Initializes asynchronous filesystem

namespace settings {
  extern Setting<int> workers_count;
}

struct Core {
  explicit Core(
      std::vector<std::string_view> const& = std::vector<std::string_view>());
  Core(int, const char* []);

  ~Core();

  static Core* instance;

  void queueTask(concur::Worker::Task t);
private:
  concur::BlockingQueue<concur::Worker::Task> tasks_;
  std::vector<std::unique_ptr<concur::Worker>> general_purpose_workers_;
};
}  // namespace slt

#endif
