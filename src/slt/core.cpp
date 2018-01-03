#include "slt/core.h"

#include "slt/debug/assert.h"
#include "slt/file/file_internal.h"
#include "slt/log/log_internal.h"
#include "slt/settings/settings_internal.h"

namespace {
std::vector<std::string_view> argvToArgs(int argc, const char* argv[]) {
  std::vector<std::string_view> args(argc - 1);
  for (int i = 1; i < argc; ++i) {
    args[i - 1] = argv[i];
  }
  return args;
}
}  // namespace

namespace slt {
    namespace settings {
      Setting<int> workers_count(4, "workers",
        "The number fo general purpose workers");
    }

Core* Core::instance = nullptr;

Core::Core(int argc, const char* argv[]) : Core(argvToArgs(argc, argv)) {}

Core::Core(std::vector<std::string_view> const& args) {
  // There should only ever be a single core instance in existence.
  SLT_ASSERT(instance == nullptr);
  instance = this;
  logging::preInit();

  settings::init(args);
  logging::init();

  file::startFilesystemThread();

  general_purpose_workers_.reserve(settings::workers_count.get());
  for (int i = 0; i < settings::workers_count.get(); ++i) {
    general_purpose_workers_.emplace_back(std::make_unique<concur::Worker>(&tasks_, "General " + std::to_string(i)));
  }
}

Core::~Core() {
  for (auto const& x: general_purpose_workers_) {
    tasks_.push(nullptr);
  }
  // Block on all workers before proceeding with the rest of the shutdown.
  general_purpose_workers_.clear();

  file::stopFilesystemThread();
  instance = nullptr;

  settings::resetAll();
  logging::shutdown();
}

void Core::queueTask(concur::Worker::Task t) {
  tasks_.push(std::move(t));
}

// void Core::update() {
// main_queue.executeAll();
//}
}  // namespace slt
